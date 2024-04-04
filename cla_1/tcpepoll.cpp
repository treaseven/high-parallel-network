#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>

void setnonblocking(int fd)
{
    //fcntl函数是一个多功能的文件描述符操作函数，可以用来获取或设置文件描述符的属性或行为
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("usage: ./tcpepoll ip port \n");
        printf("example: ./tcpepoll 192.168.31.176 5085\n\n");
        return -1;
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket() failed");
        return -1;
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt)); //允许其他套接字绑定到这个端口，除非有活动的监听套接字绑定到它
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt)); //禁用Nagle算法，减少数据传输的延迟
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt)); //允许多个套接字绑定到同一个端口号，前提是每个套接字都设置了这个选项
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt)); //启用保活机制，如果在一定时间内没有数据交换，TCP会自动发送一个保活探测分节给对方

    setnonblocking(listenfd);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    //将套接字绑定到一个特定的网络地址，使得套接字能够接收发送到该地址的数据
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind() failed");
        close(listenfd);
        return -1;
    }

    //监听来自客户端的连接请求，128监听队列的最大长度，即操作系统允许排毒等待接收的传入连接请求的最大数量
    if (listen(listenfd, 128) != 0)
    {
        perror("listen() failed");
        close(listenfd);
        return -1;
    }

    int epollfd=epoll_create(1);

    struct epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

    struct epoll_event evs[10];

    while(true)
    {
        int infds = epoll_wait(epollfd, evs, 10, -1);

        if (infds < 0)
        {
            perror("epoll_wait() failed");
            break;
        }

        if (infds == 0)
        {
            printf("epoll_wait() timeout.\n");
            continue;
        }

        for (int ii = 0; ii < infds; ii++)
        {
            if (evs[ii].data.fd == listenfd)
            {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
                setnonblocking(clientfd);

                printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

                ev.data.fd=clientfd;
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
            }
            else
            {
                if(evs[ii].events & EPOLLRDHUP)
                {
                    printf("1client(eventfs=%d) disconnected.\n", evs[ii].data.fd);
                    close(evs[ii].data.fd);
                }
                else if (evs[ii].events & (EPOLLIN|EPOLLPRI))
                {
                    char buffer[1024];
                    while(true)
                    {
                        bzero(&buffer, sizeof(buffer)); //将缓冲区的内容全部清零
                        //ssize_t是一个POSIX系统上定义的数据类型，用于标识可以返回错误的函数的返回值大小，当发生错误时，函数会返回‘-1’,并设置‘errno’来指明错误的具体类型
                        ssize_t nread = read(evs[ii].data.fd, buffer, sizeof(buffer));
                        if (nread > 0)
                        {
                            // 把接收到的报文内容原封不动的发回去
                            printf("recv(eventfd=%d):%s\n", evs[ii].data.fd, buffer);
                            send(evs[ii].data.fd, buffer, strlen(buffer), 0);
                        }
                        else if (nread == -1 && errno == EINTR) //读取数据的时候被信号中断，继续读取
                        {
                            continue;
                        }
                        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) //全部的数据已读取完毕
                        {
                            break;
                        }
                        else if (nread == 0) //客户端连接已断开
                        {
                            printf("2client(eventfd=%d) disconnected.\n", evs[ii].data.fd);
                            close(evs[ii].data.fd);
                            break;
                        }
                    }
                }
                else if (evs[ii].events & EPOLLOUT) //有数据需要写，暂时没有代码
                {

                }
                else //其他事件，都视为错误
                {
                    printf("3client(eventfd=%d) error.\n", evs[ii].data.fd);
                    close(evs[ii].data.fd);
                }
            }
        }
    }
    
    return 0;
}

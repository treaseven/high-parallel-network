#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include "InetAddress.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./tcpepoll ip port\n");
        printf("example: ./tcpepoll 192.168.31.176 5085.\n\n");
        return -1;
    }

    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket() failed");
        return -1;
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));


    /*struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));*/
    InetAddress servaddr(argv[1], atoi(argv[2]));

    if (bind(listenfd, servaddr.addr(), sizeof(sockaddr)) < 0)
    {
        perror("bind() failed");
        close(listenfd);
        return -1;
    }

    if (listen(listenfd, 128) != 0)
    {
        perror("listen() failed");
        close(listenfd);
        return -1;
    }

    int epollfd = epoll_create(1);

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
            perror("epoll_wait() failed.\n");
            break;
        }

        if (infds == 0)
        {
            printf("epoll_wait() timeout.\n");
            continue;
        }

        for (int ii = 0; ii < infds; ii++)
        {
            if (evs[ii].events & EPOLLRDHUP)
            {
                printf("client(eventfs=%d) disconnected.\n", evs[ii].data.fd);
                close(evs[ii].data.fd);
            }
            else if (evs[ii].events & (EPOLLIN | EPOLLPRI))
            {
                if (evs[ii].data.fd == listenfd)
                {
                    struct sockaddr_in peeraddr;
                    socklen_t len = sizeof(peeraddr);
                    int clientfd = accept4(listenfd, (struct sockaddr *)&peeraddr, &len, SOCK_NONBLOCK);

                    InetAddress clientaddr(peeraddr);

                    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd, clientaddr.ip(), clientaddr.port());

                    ev.data.fd = clientfd;
                    ev.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
                }
                else
                {
                    char buffer[1024];
                    while(true)
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(evs[ii].data.fd, buffer, sizeof(buffer));
                        if (nread > 0)
                        {
                            printf("recv(eventfd=%d):%s\n", evs[ii].data.fd, buffer);
                            send(evs[ii].data.fd, buffer, strlen(buffer), 0);
                        }
                        else if (nread == -1 && errno == EINTR)
                        {
                            continue;
                        }
                        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                        {
                            break;
                        }
                        else if (nread == 0)
                        {
                            printf("client(eventfd=%d) disconnected.\n", evs[ii].data.fd);
                            close(evs[ii].data.fd);
                            break;
                        }
                    }
                }
            }
            else if (evs[ii].events & EPOLLOUT)
            {

            }
            else
            {
                printf("client(eventfd=%d) error.\n", evs[ii].data.fd);
                close(evs[ii].data.fd);
            }
        }
    }
    
    
    
    return 0;
}
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
#include "Socket.h"
#include "Epoll.h"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("usage: ./tcpepoll ip port\n");
        printf("example: ./tcpepoll 192.168.31.176 5085\n");
        return -1;
    }

    Socket servsock(createnonblocking());
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true);

    servsock.bind(servaddr);
    servsock.listen();

    /*int epollfd = epoll_create(1);

    struct epoll_event ev;
    ev.data.fd  = servsock.fd();
    ev.events = EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock.fd(), &ev);

    struct epoll_event evs[10];*/

    Epoll ep;
    ep.addfd(servsock.fd(), EPOLLIN);
    std::vector<epoll_event> evs;

    while(true)
    {
        /*int infds = epoll_wait(epollfd, evs, 10, -1);

        if (infds < 0)
        {
            perror("epoll wait() failed");
            break;
        }

        if (infds == 0)
        {
            printf("epoll_wait() timeout.\n");
            continue;
        }*/
        evs = ep.loop();

        for (auto &ev:evs)
        {
            if (ev.events & EPOLLRDHUP)
            {
                printf("client(clientfd=%d) disconnected.\n", ev.data.fd);
                close(ev.data.fd);
            }
            else if (ev.events & (EPOLLIN | EPOLLPRI))
            {
                if (ev.data.fd == servsock.fd())
                {
                    InetAddress clientaddr;
                    Socket *clientsock = new Socket(servsock.accept(clientaddr));

                    printf("accept clientfd(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                    ev.data.fd = clientsock->fd();
                    ev.events = EPOLLIN|EPOLLET;
                    ep.addfd(clientsock->fd(), EPOLLIN | EPOLLET);
                }
                else
                {
                    char buffer[1024];
                    while(true)
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(ev.data.fd, buffer, sizeof(buffer));
                        if (nread > 0)
                        {
                            printf("recv(eventfd=%d):%s\n", ev.data.fd, buffer);
                            send(ev.data.fd, buffer, strlen(buffer), 0);
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
                            printf("client(eventfd=%d) disconnected.\n", ev.data.fd);
                            close(ev.data.fd);
                            break;
                        }
                    }
                }
            }
            else if (ev.events & EPOLLOUT)
            {

            }
            else
            {
                    printf("clientfd(eventfd=%d) error.\n", ev.data.fd);
                    close(ev.data.fd);
            }
        }
        
    }

    return 0;
}

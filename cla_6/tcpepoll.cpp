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
    if (argc != 3)
    {
        printf("usage: ./tcpepoll ip port\n");
        printf("example: ./tcpepoll 192.168.31.176 5085.\n\n");
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

    Epoll ep;
    //ep.addfd(servsock.fd(), EPOLLIN);
    Channel *servchannel = new Channel(&ep, servsock.fd());
    servchannel->enablereading();

    while(true)
    {
        //std::vector<epoll_event> evs;
        std::vector<Channel *> channels = ep.loop();

        for(auto &ch:channels)
        {
            if (ch->revents() & EPOLLRDHUP)
            {
                printf("client(eventfs=%d) disconnected.\n", ch->fd());
                close(ch->fd());
            }
            else if (ch->revents() & (EPOLLIN | EPOLLPRI))
            {
                if (ch == servchannel)
                {
                    InetAddress clientaddr;

                    Socket *clientsock = new Socket(servsock.accept(clientaddr));

                    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                    //ep.addfd(clientsock->fd(), EPOLLIN | EPOLLET);
                    Channel *clientchannel = new Channel(&ep, clientsock->fd());
                    clientchannel->useet();
                    clientchannel->enablereading();
                }
                else
                {
                    char buffer[1024];
                    while(true)
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(ch->fd(), buffer, sizeof(buffer));
                        if (nread > 0)
                        {
                            printf("recv(eventfd=%d):%s\n", ch->fd(), buffer);
                            send(ch->fd(), buffer, strlen(buffer), 0);
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
                            printf("client(eventfd=%d) disconnected.\n", ch->fd());
                            close(ch->fd());
                            break;
                        }
                    }
                }
            }
            else if (ch->revents() & EPOLLOUT)
            {

            }
            else
            {
                printf("client(eventfd=%d) error.\n", ch->fd());
                close(ch->fd());
            }
        }
    }
    
    
    
    return 0;
}
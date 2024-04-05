/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>*/
#include "TcpServer.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./tcpepoll ip port\n");
        printf("example: ./tcpepoll 192.168.31.176 5085.\n\n");
        return -1;
    }

    /*Socket servsock(createnonblocking());
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true);
    servsock.bind(servaddr);
    servsock.listen();

    EventLoop loop;
    Channel *servchannel = new Channel(&loop, servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconection, servchannel, &servsock));
    servchannel->enablereading();*/

    TcpServer tcpserver(argv[1], atoi(argv[2]));

    tcpserver.start();
    return 0;
}
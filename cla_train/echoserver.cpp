#include "EchoServer.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./echoserver ip port\n");
        printf("example: ./echoserver 192.168.31.176 5085.\n\n");
        return -1;
    }

    //TcpServer tcpserver(argv[1], atoi(argv[2]));
    //tcpserver.start();
    EchoServer echoserver(argv[1], atoi(argv[2]), 3, 0);
    echoserver.Start();
    return 0;
}
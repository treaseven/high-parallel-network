#include "EchoServer.h"
#include <signal.h>

EchoServer *echoserver;

void Stop(int sig)
{
    echoserver->Stop();
    printf("sig=%d\n", sig);
    printf("echoserver已停止。\n");
    delete echoserver;
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./echoserver ip port\n");
        printf("example: ./echoserver 192.168.190.131 5085.\n\n");
        return -1;
    }

    signal(SIGTERM, Stop);
    signal(SIGINT, Stop);

    echoserver = new EchoServer(argv[1], atoi(argv[2]), 3, 2);
    echoserver->Start();
    return 0;
}
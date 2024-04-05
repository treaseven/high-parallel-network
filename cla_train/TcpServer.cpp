#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    /*Socket *servsock = new Socket((createnonblocking()));
    InetAddress servaddr(ip, port);
    servsock->setreuseaddr(true);
    servsock->settcpnodelay(true);
    servsock->setreuseport(true);
    servsock->setkeepalive(true);
    servsock->bind(servaddr);
    servsock->listen();

    Channel *servchannel = new Channel(&loop_, servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconection, servchannel, servsock));
    servchannel->enablereading();*/
    acceptor_ = new Acceptor(&loop_, ip, port);
}

TcpServer::~TcpServer()
{
    delete acceptor_;
}

void TcpServer::start()
{
    loop_.run();
}
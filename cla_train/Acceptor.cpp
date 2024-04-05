#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *Loop, const std::string &ip, const uint16_t port):loop_(Loop)
{
    servsock_ = new Socket((createnonblocking()));
    InetAddress servaddr(ip, port);
    servsock_->setreuseaddr(true);
    servsock_->settcpnodelay(true);
    servsock_->setreuseport(true);
    servsock_->setkeepalive(true);
    servsock_->bind(servaddr);
    servsock_->listen();

    acceptchannel_ = new Channel(loop_, servsock_->fd());
    acceptchannel_->setreadcallback(std::bind(&Channel::newconection, acceptchannel_, servsock_));
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}
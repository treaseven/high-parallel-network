#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage, clientchannel_));
    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}
#include "Acceptor.h"
#include "Connection.h"

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
    //acceptchannel_->setreadcallback(std::bind(&Channel::newconection, acceptchannel_, servsock_));
    acceptchannel_->setreadcallback(std::bind(&Acceptor::newconection, this));
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}

void Acceptor::newconection()
{
    InetAddress clientaddr;

    Socket *clientsock = new Socket(servsock_->accept(clientaddr));

    //printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

    newconnectioncb_(clientsock);
}

void Acceptor::setnewconnectioncb(std::function<void(Socket *)> fn)
{
    newconnectioncb_ = fn;
}
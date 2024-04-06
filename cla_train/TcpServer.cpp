#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconection, this, std::placeholders::_1));
    loop_.setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    for (auto &aa:conns_)
    {
        delete aa.second;
    }
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newconection(Socket *clientsock)
{
    Connection *conn = new Connection(&loop_, clientsock);

    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));

    conns_[conn->fd()] = conn;

    if (newconectioncb_) newconectioncb_(conn);
}

void TcpServer::closeconnection(Connection *conn)
{
    if (closeconectioncb_) closeconectioncb_(conn);
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection *conn)
{
    if (errorconectioncb_) errorconectioncb_(conn);
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection *conn, std::string message)
{
    if (onmessagecb_) onmessagecb_(conn, message);
}

void TcpServer::sendcomplete(Connection *conn)
{
    printf("send complete.\n");

    if (sendcompletecb_) sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop *loop)
{
    if (timeoutcb_) timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(Connection *)> fn)
{
    newconectioncb_ = fn;
}

void TcpServer::setcloseconnectioncb(std::function<void(Connection *)> fn)
{
    closeconectioncb_ = fn;
}

void TcpServer::seterrorconnectioncb(std::function<void(Connection *)> fn)
{
    errorconectioncb_ = fn;
}

void TcpServer::setonmessagecb(std::function<void(Connection *, std::string &message)> fn)
{
    onmessagecb_ = fn;
}

void TcpServer::setsendcompletecb(std::function<void(Connection *)> fn)
{
    sendcompletecb_ = fn;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop *)> fn)
{
    timeoutcb_ = fn;
}
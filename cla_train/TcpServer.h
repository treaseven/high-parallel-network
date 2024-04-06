#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include <map>

class TcpServer
{
private:
    EventLoop loop_;
    Acceptor *acceptor_;
    std::map<int, Connection*> conns_;
    std::function<void(Connection *)> newconectioncb_;
    std::function<void(Connection *)> closeconectioncb_;
    std::function<void(Connection *)> errorconectioncb_;
    std::function<void(Connection *, std::string &message)> onmessagecb_;
    std::function<void(Connection *)> sendcompletecb_;
    std::function<void(EventLoop *)> timeoutcb_;
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

    void start();

    void newconection(Socket *clientsock);
    void closeconnection(Connection *conn);
    void errorconnection(Connection *conn);
    void onmessage(Connection *conn, std::string& message);
    void sendcomplete(Connection *conn);
    void epolltimeout(EventLoop *loop);

    void setnewconnectioncb(std::function<void(Connection *)> fn);
    void setcloseconnectioncb(std::function<void(Connection *)> fn);
    void seterrorconnectioncb(std::function<void(Connection *)> fn);
    void setonmessagecb(std::function<void(Connection *, std::string &message)> fn);
    void setsendcompletecb(std::function<void(Connection *)> fn);
    void settimeoutcb(std::function<void(EventLoop *)> fn);
};
#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"

class TcpServer
{
private:
    EventLoop loop_;
    Acceptor *acceptor_;
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

    void start();
};
#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Acceptor
{
private:
    EventLoop *loop_;
    Socket *servsock_;
    Channel *acceptchannel_;
public:
    Acceptor(EventLoop *Loop, const std::string &ip, const uint16_t port);
    ~Acceptor();
};
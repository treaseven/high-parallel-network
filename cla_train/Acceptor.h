#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Connection.h"
#include <memory>

class Acceptor
{
private:
    EventLoop* loop_;
    Socket servsock_;
    Channel acceptchannel_;
    std::function<void(std::unique_ptr<Socket>)> newconnectioncb_;
public:
    Acceptor(EventLoop* Loop, const std::string &ip, const uint16_t port);
    ~Acceptor();

    void newconection();

    void setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn);
};
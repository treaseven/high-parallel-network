#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Timestamp.h"
#include <memory>
#include <atomic>

class EventLoop;
class Channel;
class Connection;
using spConnection=std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* loop_;
    std::unique_ptr<Socket> clientsock_;
    std::unique_ptr<Channel> clientchannel_;
    Buffer inputbuffer_;
    Buffer outputbuffer_;
    std::atomic_bool disconnect_;
    std::function<void(spConnection)> closecallback_;
    std::function<void(spConnection)> errorcallback_;
    std::function<void(spConnection, std::string&)> onmessagecallback_;
    std::function<void(spConnection)> sendcompletecallback_;
    Timestamp lastatime_;
public:
    Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void onmessage();
    void closecallback();
    void errorcallback();
    void writecallback();

    void setclosecallback(std::function<void(spConnection)> fn);
    void seterrorcallback(std::function<void(spConnection)> fn);
    void setonmessagecallback(std::function<void(spConnection, std::string&)> fn);
    void setsendcompletecallback(std::function<void(spConnection)> fn);

    void send(const char *data, size_t size);
    void sendinloop(const char *data, size_t size);

    bool timeout(time_t now, int val);
};
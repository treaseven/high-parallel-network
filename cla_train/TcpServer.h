#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include <map>
#include <memory>
#include <mutex>

class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainloop_;
    std::vector<std::unique_ptr<EventLoop>> subloops_;
    Acceptor acceptor_;
    int threadnum_;
    ThreadPool threadpool_;
    std::mutex mmtuex_;
    std::map<int, spConnection> conns_;
    std::function<void(spConnection)> newconectioncb_;
    std::function<void(spConnection)> closeconectioncb_;
    std::function<void(spConnection)> errorconectioncb_;
    std::function<void(spConnection, std::string &message)> onmessagecb_;
    std::function<void(spConnection)> sendcompletecb_;
    std::function<void(EventLoop *)> timeoutcb_;
public:
    TcpServer(const std::string &ip, const uint16_t port, int threadnum=3);
    ~TcpServer();

    void start();

    void newconection(std::unique_ptr<Socket> clientsock);
    void closeconnection(spConnection conn);
    void errorconnection(spConnection conn);
    void onmessage(spConnection conn, std::string& message);
    void sendcomplete(spConnection conn);
    void epolltimeout(EventLoop *loop);

    void setnewconnectioncb(std::function<void(spConnection)> fn);
    void setcloseconnectioncb(std::function<void(spConnection)> fn);
    void seterrorconnectioncb(std::function<void(spConnection)> fn);
    void setonmessagecb(std::function<void(spConnection, std::string &message)> fn);
    void setsendcompletecb(std::function<void(spConnection)> fn);
    void settimeoutcb(std::function<void(EventLoop *)> fn);

    void removeconn(int fd);
};
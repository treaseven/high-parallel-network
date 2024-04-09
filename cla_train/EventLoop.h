#pragma once
#include <functional>
#include "Epoll.h"
#include <memory>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <map>
#include <atomic>
#include "Connection.h"

class Channel;
class Epoll;
class Connection;
using spConnection=std::shared_ptr<Connection>;

class EventLoop
{
private:
    int timetvl_;
    int timeout_;
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop *)> epolltimeoutcallback_;
    pid_t threadid_;
    std::queue<std::function<void()>> taskqueue_;
    std::mutex mutex_;
    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_;
    int timerfd_;
    std::unique_ptr<Channel> timerchannel_;
    bool mainloop_;
    std::mutex mmtuex_;
    std::map<int, spConnection> conns_;
    std::function<void(int)> timercallback_;
    std::atomic_bool stop_;
public:
    EventLoop(bool mainloop, int timetvl=30, int timeout=80);
    ~EventLoop();

    void run();
    void stop();

    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop *)> fn);

    bool isinloopthread();

    void queueinloop(std::function<void()> fn);
    void wakeup();
    void handlewakeup();

    void handletimer();

    void newconnection(spConnection conn);
    void settimercallback(std::function<void(int)> fn);
};
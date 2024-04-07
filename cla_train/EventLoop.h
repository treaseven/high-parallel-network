#pragma once
#include <functional>
#include "Epoll.h"
#include <memory>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

class Channel;
class Epoll;

class EventLoop
{
private:
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
public:
    EventLoop(bool mainloop);
    ~EventLoop();

    void run();

    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop *)> fn);

    bool isinloopthread();

    void queueinloop(std::function<void()> fn);
    void wakeup();
    void handlewakeup();

    void handletimer();
};
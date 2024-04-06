#pragma once
#include <functional>
#include "Epoll.h"

class Channel;
class Epoll;

class EventLoop
{
private:
    Epoll *ep_;
    std::function<void(EventLoop *)> epolltimeoutcallback_;
public:
    EventLoop();
    ~EventLoop();

    void run();

    void updatechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop *)> fn);
};
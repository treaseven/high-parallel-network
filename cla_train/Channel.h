#pragma once
#include <sys/epoll.h>
#include <functional>
#include "Socket.h"
#include "EventLoop.h"

class EventLoop;

class Channel
{
private:
    int fd_ = -1;
    //Epoll *ep_ = nullptr;
    EventLoop *loop_ = nullptr;
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    std::function<void()> readcallback_;
    std::function<void()> closecallback_;
    std::function<void()> errorcallback_;
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    int fd();
    void useet();
    void enablereading();
    void setinepoll();
    void setrevents(uint32_t ev);
    bool inpoll();
    uint32_t events();
    uint32_t revents();

    void handleevent();

    //void onmessage();
    void setreadcallback(std::function<void()> fn);

    void setclosecallback(std::function<void()> fn);
    void seterrorcallback(std::function<void()> fn);
};
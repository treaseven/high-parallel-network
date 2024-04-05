#pragma once
#include <sys/epoll.h>
#include "Epoll.h"
#include "Socket.h"

class Epoll;

class Channel
{
private:
    int fd_ = -1;
    Epoll *ep_ = nullptr;
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    bool islisten_ = false;
public:
    Channel(Epoll* ep, int fd, bool islisten);
    ~Channel();

    int fd();
    void useet();
    void enablereading();
    void setinepoll();
    void setrevents(uint32_t ev);
    bool inpoll();
    uint32_t events();
    uint32_t revents();

    void handleevent(Socket *servsock);
};
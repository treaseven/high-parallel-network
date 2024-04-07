#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
//#include "Connection.h"

Channel::Channel(EventLoop* loop, int fd):loop_(loop), fd_(fd)
{

}

Channel::~Channel()
{

}

int Channel::fd()
{
    return fd_;
}

void Channel::useet()
{
    events_ = events_ | EPOLLET;
}

void Channel::enablereading()
{
    events_ |= EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::disablereading()
{
    events_ &= ~EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::enablewriting()
{
    events_ |= EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disablewriting()
{
    events_ &= ~EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disableall()
{
    events_ = 0;
    loop_->updatechannel(this); 
}

void Channel::remove()
{
    disableall();
    loop_->removechannel(this);
}

void Channel::setinepoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}

bool Channel::inpoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}

uint32_t Channel::revents()
{
    return revents_;
}

void Channel::handleevent()
{
    if (revents_ & EPOLLRDHUP)
    {
        printf("EPOLLRDHUP.\n");
        //remove();
        closecallback_();
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        printf("EPOLLIN | EPOLLPRI.\n");
        readcallback_();
    }
    else if (revents_ & EPOLLOUT)
    {
        printf("EPOLLOUT.\n");
        writecallback_();
    }
    else
    {
        printf("others.\n");
        //remove();
        errorcallback_();
    }
}

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}

void Channel::setclosecallback(std::function<void()> fn)
{
    closecallback_ = fn;
}

void Channel::seterrorcallback(std::function<void()> fn)
{
    errorcallback_ = fn;
}

void Channel::setwritecallback(std::function<void()> fn)
{
    writecallback_ = fn;
}
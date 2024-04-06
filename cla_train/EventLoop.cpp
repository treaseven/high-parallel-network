#include "EventLoop.h"
#include <unistd.h>
#include <sys/syscall.h>

EventLoop::EventLoop():ep_(new Epoll)
{

}
EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::run()
{
    //printf("EvnetLoop::run() thread is %ld.\n", syscall(SYS_gettid));
    while(true)
    {
        std::vector<Channel *> channels = ep_->loop(10 * 1000);

        if (channels.size() == 0)
        {
            epolltimeoutcallback_(this);
        }
        else
        {
            for(auto &ch:channels)
            {
                ch->handleevent();
            }
        }
    } 
}

void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop *)> fn)
{
    epolltimeoutcallback_ = fn;
}
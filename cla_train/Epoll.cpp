#include "Epoll.h"

Epoll::Epoll()
{
    if((epollfd_=epoll_create(1)) == -1)
    {
        printf("Epoll_crate() failed(%d).\n ", errno);
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_);
}

/*void Epoll::addfd(int fd, uint32_t op)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = op;

    if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        printf("epoll_ctl() failed(%d).\n", errno);
        exit(-1);
    }
}*/

void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();

    if (ch->inpoll())
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl() failed.\n");
            exit(-1);
        }
    }
    else
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl() failed.\n");
            exit(-1);
        }
        ch->setinepoll();
    }
}

/*std::vector<epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;

    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout);
        
    if (infds < 0)
    {
        perror("epoll_wait() failed.\n");
        exit(-1);
    }

    if (infds == 0)
    {
        printf("epoll_wait() timeout.\n");
        return evs;
    }

    for (int ii = 0; ii < infds; ii++)
    {
        evs.push_back(events_[ii]);
    }

    return evs;
}*/

std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *> channels;

    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout);
        
    if (infds < 0)
    {
        perror("epoll_wait() failed.\n");
        exit(-1);
    }

    if (infds == 0)
    {
        //printf("epoll_wait() timeout.\n");
        return channels;
    }

    for (int ii = 0; ii < infds; ii++)
    {
        //evs.push_back(events_[ii]);
        Channel *ch=(Channel *)events_[ii].data.ptr;
        ch->setrevents(events_[ii].events);
        channels.push_back(ch);
    }

    return channels;
}
#include "Epoll.h"

Epoll::Epoll()
{
    epollfd_ = epoll_create(1);
}

Epoll::~Epoll()
{
    close(epollfd_);
}

void Epoll::addfd(int fd, uint32_t op)
{
    struct epoll_event ev;
    ev.data.fd  = fd;
    ev.events = EPOLLIN;

    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        printf("epoll_ctl() failed(%d).\n", errno);
        exit(-1);
    }
}

std::vector<epoll_event> Epoll::loop(int timeout)
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

    for(int ii = 0; ii < infds; ii++)
    {
        evs.push_back(events_[ii]);
    }

    return evs;
}
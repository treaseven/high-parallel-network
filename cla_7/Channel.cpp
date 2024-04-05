#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"

Channel::Channel(Epoll* ep, int fd, bool islisten):ep_(ep), fd_(fd), islisten_(islisten)
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
    ep_->updatechannel(this);
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

void Channel::handleevent(Socket *servsock)
{
    if (revents_ & EPOLLRDHUP)
    {
        printf("client(eventfs=%d) disconnected.\n", fd_);
        close(fd_);
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (islisten_ == true)
        {
            InetAddress clientaddr;

            Socket *clientsock = new Socket(servsock->accept(clientaddr));

            printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

            Channel *clientchannel = new Channel(ep_, clientsock->fd(), false);
            clientchannel->useet();
            clientchannel->enablereading();
        }
        else
        {
            char buffer[1024];
            while(true)
            {
                bzero(&buffer, sizeof(buffer));
                ssize_t nread = read(fd_, buffer, sizeof(buffer));
                if (nread > 0)
                {
                    printf("recv(eventfd=%d):%s\n", fd_, buffer);
                    send(fd_, buffer, strlen(buffer), 0);
                }
                else if (nread == -1 && errno == EINTR)
                {
                    continue;
                }
                else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                {
                    break;
                }
                else if (nread == 0)
                {
                    printf("client(eventfd=%d) disconnected.\n", fd_);
                    close(fd_);
                    break;
                }
            }
        }
    }
    else if (revents_ & EPOLLOUT)
    {

    }
    else
    {
        printf("client(eventfd=%d) error.\n", fd_);
        close(fd_);
    }
}
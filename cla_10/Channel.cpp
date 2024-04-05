#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"

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
    //ep_->updatechannel(this);
    loop_->updatechannel(this);
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
        printf("client(eventfs=%d) disconnected.\n", fd_);
        close(fd_);
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        /*if (islisten_ == true)
            newconection(servsock);
        else
            onmessage();*/

        readcallback_();
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

void Channel::newconection(Socket *servsock)
{
    InetAddress clientaddr;

    Socket *clientsock = new Socket(servsock->accept(clientaddr));

    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

    Channel *clientchannel = new Channel(loop_, clientsock->fd());
    clientchannel->setreadcallback(std::bind(&Channel::onmessage, clientchannel));
    clientchannel->useet();
    clientchannel->enablereading();
}

void Channel::onmessage()
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

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}
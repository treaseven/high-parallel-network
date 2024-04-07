#include "Connection.h"
#include <sys/syscall.h>

Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock)
            :loop_(loop),clientsock_(std::move(clientsock)),disconnect_(false),clientchannel_(new Channel(loop_, clientsock_->fd()))
{
    //clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback, this));
    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    //delete clientsock_;
    //delete clientchannel_;
    //printf("Connection对象已析构.\n");
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}


void Connection::closecallback()
{
    disconnect_ = true;
    clientchannel_->remove();
    closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
    disconnect_ = true;
    clientchannel_->remove();
    errorcallback_(shared_from_this());
}

void Connection::setclosecallback(std::function<void(spConnection)> fn)
{
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(spConnection)> fn)
{
    errorcallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(spConnection, std::string &)> fn)
{
    onmessagecallback_ = fn;
}

void Connection::setsendcompletecallback(std::function<void(spConnection)> fn)
{
    sendcompletecallback_ = fn;
}

void Connection::onmessage()
{
    char buffer[1024];
    while(true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));
        if (nread > 0)
        {
            inputbuffer_.append(buffer, nread);
        }
        else if (nread == -1 && errno == EINTR)
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            while(true)
            {
                int len;
                memcpy(&len, inputbuffer_.data(), 4);
                if(inputbuffer_.size()<len+4) break;

                std::string message(inputbuffer_.data()+4, len);
                inputbuffer_.erase(0, len+4);

                printf("message(eventfd=%d):%s\n", fd(), message.c_str());
                
                onmessagecallback_(shared_from_this(), message);
            }
            break;
        }
        else if (nread == 0)
        {
            closecallback();
            break;
        }
    }
}

void Connection::send(const char *data, size_t size)
{
    if (disconnect_ == true) {printf("客户端连接已经断开了，send()直接返回.\n"); return;}

    if (loop_->isinloopthread())
    {
        printf("send() 在事件循环的线程中.\n");
        sendinloop(data, size);
    }
    else
    {
        printf("send() 不在事件循环的线程中.\n");
        loop_->queueinloop(std::bind(&Connection::sendinloop,this, data, size));
    }
}

void Connection::sendinloop(const char *data, size_t size)
{
    outputbuffer_.appendwithhead(data, size);
    clientchannel_->enablewriting();
}

void Connection::writecallback()
{
    int writen=::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if (writen>0) outputbuffer_.erase(0, writen);

    //printf("Connection::writecallback() thread is %ld.\n", syscall(SYS_gettid));
    if(outputbuffer_.size() == 0) 
    {
        clientchannel_->disablewriting();
        sendcompletecallback_(shared_from_this());
    }
}
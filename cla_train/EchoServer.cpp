#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port, int threadnum):tcpserver_(ip, port, threadnum)
{
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    //tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}

void EchoServer::Start()
{
    tcpserver_.start();
}

void EchoServer::HandleNewConection(Connection *conn)
{
    std::cout << "New Connection Come in" << std::endl;
    printf("void EchoServer::HandleNewConection() thread is %ld.\n", syscall(SYS_gettid));
}

void EchoServer::HandleClose(Connection *conn)
{
    std::cout << "EchoServer conn close" << std::endl;
}

void EchoServer::HandleError(Connection *conn)
{
    std::cout << "EchoServer conn error" << std::endl;
}

void EchoServer::HandleMessage(Connection *conn, std::string& message)
{
    printf("void EchoServer::HandleMessage() thread is %ld.\n", syscall(SYS_gettid));
    message = "reply:" + message;

    conn->send(message.data(), message.size());
}

void EchoServer::HandleSendComplete(Connection *conn)
{
    std::cout << "EchoServer send complete" << std::endl;
}

/*void EchoServer::HandleTimeOut(EventLoop *loop)
{
    std::cout << "EchoServer  timeout" << std::endl;
}*/
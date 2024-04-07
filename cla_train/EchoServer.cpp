#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port, int subthreadnum, int workthreadnum)
    :tcpserver_(ip, port, subthreadnum), threadpool_(workthreadnum, "WORKS")
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

void EchoServer::HandleNewConection(spConnection conn)
{
    std::cout << "New Connection Come in" << std::endl;
    //printf("void EchoServer::HandleNewConection() thread is %ld.\n", syscall(SYS_gettid));
}

void EchoServer::HandleClose(spConnection conn)
{
    std::cout << "EchoServer conn close" << std::endl;
}

void EchoServer::HandleError(spConnection conn)
{
    std::cout << "EchoServer conn error" << std::endl;
}

void EchoServer::HandleMessage(spConnection conn, std::string& message)
{
    //printf("void EchoServer::HandleMessage() thread is %ld.\n", syscall(SYS_gettid));
    
    threadpool_.addtask(std::bind(&EchoServer::OnMessage, this, conn, message));
}

void EchoServer::OnMessage(spConnection conn, std::string& message)
{
    message = "reply:" + message;
    conn->send(message.data(), message.size());
}

void EchoServer::HandleSendComplete(spConnection conn)
{
    std::cout << "Message send complete" << std::endl;
}

/*void EchoServer::HandleTimeOut(EventLoop *loop)
{
    std::cout << "EchoServer  timeout" << std::endl;
}*/
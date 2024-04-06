#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port):tcpserver_(ip, port)
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
}

void EchoServer::HandleClose(Connection *conn)
{
    std::cout << "EchoServer conn close" << std::endl;
}

void EchoServer::HandleError(Connection *conn)
{
    std::cout << "EchoServer conn error" << std::endl;
}

void EchoServer::HandleMessage(Connection *conn, std::string message)
{
    message = "reply:" + message;

    int len = message.size();
    std::string tmpbuf((char *)&len, 4);
    tmpbuf.append(message);

    //send(conn->fd(), tmpbuf.data(), tmpbuf.size(), 0);
    conn->send(tmpbuf.data(), tmpbuf.size());
}

void EchoServer::HandleSendComplete(Connection *conn)
{
    std::cout << "EchoServer send complete" << std::endl;
}

/*void EchoServer::HandleTimeOut(EventLoop *loop)
{
    std::cout << "EchoServer  timeout" << std::endl;
}*/
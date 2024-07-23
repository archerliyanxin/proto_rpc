#include "TcpConncetion.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
namespace network{
    static EventLoop *CheckNULL(EventLoop *loop){
        if(nullptr == loop){
            exit(-1);
        }
        return loop;
    }

    TcpConncetion::TcpConncetion(EventLoop *loop, const std::string &name,
                  int sockfd,
                  const InetAddress &localAddr,
                  const InetAddress &perAddr)
    : loop_(CheckNULL(loop))
    , name_(name)
    , state_(kConnected)
    , reading_(false)
    , socket_(new Socket(sockfd))
    , channel_(new Channel(loop, sockfd))
    , localAddr_(localAddr)
    , perAddr_(perAddr)
    , highWaterSize_(64*1024*1024)
    {
        //给channel设置相应的回调函数， poller给channel通知感兴趣的事件，调用相应的操作函数
        channel_->setReadCallback(std::bind(&TcpConncetion::handleRead, this));
        channel_->setWriteCallback(std::bind(&TcpConncetion::handleWrite, this));
        channel_->setCloseCallback(std::bind(&TcpConncetion::handleClose, this));
        channel_->setErrorCallback(std::bind(&TcpConncetion::handleError, this));
        socket_->keepAlive(true);
    }

    void TcpConncetion::handleRead()
    {
            
    }
    void TcpConncetion::handleWrite()
    {

    }
    void TcpConncetion::handleClose()
    {

    }
    void TcpConncetion::handleError()
    {

    }

} 
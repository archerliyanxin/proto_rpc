#include "Acceptor.h"
#include "SocketsOps.h"
#include "InetAddress.h"
#include "Channel.h"
namespace network{
    Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, int reusePort)
    : loop_(loop)
    , acceptSock_(sockets::createNoBlockSocket(listenAddr.get_socketAddr()->sin_family))
    , acceptChannel_(loop,acceptSock_.fd())
    , listening_(false)
    , idleFd(::open("/dev/null", O_RDONLY|O_CLOEXEC))
    {
        acceptSock_.setReuseAddr(true);
        acceptSock_.setReusePort(true);
        acceptSock_.bindAddress(listenAddr);
        setNewConnectionCallBack(std::bind(&Acceptor::handleRead, this));
    }
    Acceptor::~Acceptor(){
        acceptChannel_.disableAll();
        acceptChannel_.remove();
//        ::close(acceptSock_.fd());
    }
    void Acceptor::listen(){
        listening_ = true;
        acceptSock_.listen();
        acceptChannel_.enableReading();
    }
    void Acceptor::handleRead(){
        InetAddress perAddr;
        int con_fd = acceptSock_.accept(&perAddr);
        if(con_fd > 0){
            if(cb_){
                cb_(acceptSock_.fd(),perAddr);
            } else{
                ::close(acceptSock_.fd());
            }
        }else{
            if(con_fd == EMFILE){
                ::close(idleFd);
                idleFd = ::accept(acceptSock_.fd(), NULL, NULL);
                ::close(idleFd);
                ::open("/dev/null",O_RDONLY|O_CLOEXEC);
            }
        }
    }
}
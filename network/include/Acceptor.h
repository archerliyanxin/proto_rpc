#pragma once
#include "functional"
#include "Socket.h"
#include "Channel.h"
namespace network{
    class EventLoop;
    class InetAddress;
    class Acceptor{
    public:
        using NewConnectionCallBack = std::function<void(int sock_fd, const InetAddress &)>;
        Acceptor(EventLoop *loop, const InetAddress &listenAddr, int reusePort);
        ~Acceptor();

        void setNewConnectionCallBack(const NewConnectionCallBack &cb){cb_ = std::move(cb);}
        void listen();
        bool listening(){return listening_;}
    private:
        void handleRead();
        EventLoop *loop_;
        Socket acceptSock_;
        Channel acceptChannel_;
        NewConnectionCallBack cb_;
        bool listening_;
        int idleFd;
    };
}

#pragma once
#include "Callback.h"
#include "memory"
#include "InetAddress.h"
#include "Buffer.h"
#include "atomic"
namespace network{
    /**
     * tcpserver -> acceptor -> 新用户连接 -> confd
     * confd 打包成tcpconnection 设置回调 -> channel -> poller -> channel 回调操作
     * */
    class EventLoop;
    class Socket;
    class Channel;
    class InetAddress;
    class TcpConncetion: public std::enable_shared_from_this<TcpConncetion>{
    public:
        TcpConncetion(EventLoop *loop, const std::string &name,
                      int sockfd,
                      const InetAddress &localAddr,
                      const InetAddress &perAddr);
        ~TcpConncetion();

        EventLoop *getLoop() const{ return loop_;}
        const std::string &name(){ return name_; }
        const InetAddress &localAddr(){ return localAddr_; }
        const InetAddress &perAddr(){ return perAddr_; }
        bool connected() const {return kConnected == state_; }

        void setConnectionCallBack(ConnectionCallBack &cb){ connectionCallBack_ = cb;}
        void setMessageCallBack(MessageCallBack &cb){ messageCallBack_ = cb;}
        void setWriteCompleteCallBack(WriteCompleteCallBack &cb){ writeCompleteCallBack_ = cb;}
        void setHightWaterMarkCallBack(HightWaterMarkCallBack &cb, size_t hightWaterSize){ hightWaterMarkCallBack_ = cb; highWaterSize_ = hightWaterSize;}
        void setCloseCallBack(CloseCallBack &cb){ closeCallBack_ = cb;}

        void send(const void *message, size_t len);

        void shutdown();
        void connectEstablish();
        void connectDestroyed();
    private:
        void handleRead();
        void handleWrite();
        void handleClose();
        void handleError();

        void sendInLoop(const void *message, size_t len);
        void shutdownInLoop();
    private:
        enum StateE{kDisconnected, kDisconnecting, kConnected, kConnecting};
        EventLoop *loop_;
        const std::string name_;

        std::atomic_int state_;
        bool reading_;
        // acceptor --> mainLoop | tcpConnection --> subLoop
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        const InetAddress localAddr_;
        const InetAddress perAddr_;

        ConnectionCallBack connectionCallBack_;
        MessageCallBack messageCallBack_;
        WriteCompleteCallBack writeCompleteCallBack_;
        HightWaterMarkCallBack hightWaterMarkCallBack_;
        CloseCallBack closeCallBack_;
        size_t highWaterSize_;

        Buffer inputBuffer_;
        Buffer outputBuffer_;
    };
}

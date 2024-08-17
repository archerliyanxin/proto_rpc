#pragma once
#include "TcpConncetion.h"
#include "functional"
#include "unordered_map"
#include "string"
#include "atomic"
#include <map>
#include "InetAddress.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "SocketsOps.h"
namespace network{
    class Acceptor;
    class EventLoop;
    class EventLoopThreadPool;
    class InetAddress;
    class TcpServer{
    public:
        using ThreadInitCallBack = std::function<void(EventLoop *)>;

        enum Option{
            kNoReusePort,
            kReusePort
        };
        TcpServer(EventLoop *loop, const InetAddress &address, const std::string &name, Option option = kNoReusePort);

        ~TcpServer();

        void set_thread_init_callback(const ThreadInitCallBack &callBack){threadInitCallBack_ = callBack;}
        void set_connection_callback(const ConnectionCallBack &callBack){connectionCallBack_ = callBack;}
        void set_message_callback(const MessageCallBack &messageCallBack){messageCallBack_ = messageCallBack;}
        void set_write_complete_callback(const WriteCompleteCallBack &cb){writeCompleteCallBack_ = cb;}

        void set_thread_num(int thread_num);
        void start();
    private:
        void new_connection(int socket_fd, const InetAddress &perAddr);
        void remove_connection(const TcpConncetionPtr &conn);
        void remove_connection_inLoop(const TcpConncetionPtr &conn);
    private:
        EventLoop *loop_; //用户自己定义的base loop,传进来
        const std::string ip_port_;
        const std::string name_;
        std::unique_ptr<Acceptor> acceptor_;
        std::shared_ptr<EventLoopThreadPool> threadPool_;
        ConnectionCallBack connectionCallBack_;
        MessageCallBack messageCallBack_;
        WriteCompleteCallBack writeCompleteCallBack_;
        ThreadInitCallBack threadInitCallBack_; //loop线程池初始化回调
        std::atomic_int32_t start_;

        using ConnectionMap = std::unordered_map<std::string, TcpConncetionPtr>;
        int next_conn_id_;
        ConnectionMap connectionMap_; //保存所有的连接
    };
}

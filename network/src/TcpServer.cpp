#include "TcpServer.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
namespace network{
    EventLoop* CHECK_NOT_NULL(EventLoop *loop){
        if(nullptr == loop){
            ::exit(-1);
        }
        return loop;
    }
    TcpServer::TcpServer(network::EventLoop *loop, const network::InetAddress &listenAddr, const std::string &name, network::TcpServer::Option option)
        :loop_(CHECK_NOT_NULL(loop))
          , ip_port_(listenAddr.to_ip_port())
          , acceptor_(new Acceptor(loop, listenAddr, option == kNoReusePort))
          , threadPool_(new EventLoopThreadPool(loop, name_))
          , connectionCallBack_()
          , messageCallBack_()
          , next_conn_id_(1)
    {
        acceptor_->setNewConnectionCallBack(std::bind(&TcpServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));
    }

    void TcpServer::new_connection(int socket_fd, const network::InetAddress &perAddr) {

    }

    void TcpServer::set_thread_num(int thread_num) {
        threadPool_->setThreadNum(thread_num);
    }

    void TcpServer::start() {
        if(start_++ == 0){
            threadPool_->start();
            loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
        }
    }
}

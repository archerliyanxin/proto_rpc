
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
          , start_(0)
    {
        acceptor_->setNewConnectionCallBack(std::bind(&TcpServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));
    }

    void TcpServer::new_connection(int socket_fd, const network::InetAddress &perAddr) {
        EventLoop *ioLoop = threadPool_->getNextLoop();
        char subName[64] = {0};
        snprintf(subName,sizeof subName, "-%s#%d", ip_port_.c_str(), next_conn_id_);
        next_conn_id_++;
        std::string conName = name_ + subName;

        InetAddress localAddr(sockets::getLocalAddr(socket_fd));

        // 回调都是用户设置给tcpServer -> tcpConnection -> channel -> poller ->notify channel
        TcpConncetionPtr con(new TcpConncetion(ioLoop, conName, socket_fd, localAddr, perAddr));
        connectionMap_[conName] = con;

        con->setConnectionCallBack(connectionCallBack_);
        con->setMessageCallBack(messageCallBack_);
        con->setWriteCompleteCallBack(writeCompleteCallBack_);

        con->setCloseCallBack(std::bind(&TcpServer::remove_connection, this, std::placeholders::_1));

        ioLoop->runInLoop(std::bind(&TcpConncetion::connectEstablish, con));
    }

    void TcpServer::remove_connection(const TcpConncetionPtr &conn){
        loop_->runInLoop(std::bind(&TcpServer::remove_connection_inLoop, this, conn));
    }

    void TcpServer::remove_connection_inLoop(const TcpConncetionPtr &conn){
        connectionMap_.erase(conn->name());
        EventLoop *ioLoop = conn->getLoop();
        ioLoop->queueInLoop(std::bind(&TcpConncetion::connectDestroyed, conn));

    }

    void TcpServer::set_thread_num(int thread_num) {
        threadPool_->setThreadNum(thread_num);
    }

    TcpServer::~TcpServer(){
        for (auto &item: connectionMap_) {
            TcpConncetionPtr conn(item.second);
            item.second.reset();
            conn->getLoop()->runInLoop(std::bind(&TcpConncetion::connectDestroyed, conn));
        }
    }

    void TcpServer::start() {
        if(start_++ == 0){
            threadPool_->start();
            loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
        }
    }
}

#include "TcpClient.h"


#include <stdio.h>  // snprintf

#include <cassert>


#include "Connector.h"
#include "EventLoop.h"
#include "SocketsOps.h"

using namespace network;

// TcpClient::TcpClient(EventLoop* loop)
//   : loop_(loop)
// {
// }

// TcpClient::TcpClient(EventLoop* loop, const std::string& host, uint16_t port)
//   : loop_(CHECK_NOTNULL(loop)),
//     serverAddr_(host, port)
// {
// }

namespace network {
    namespace detail {

        void removeConnection(EventLoop *loop, const TcpConncetionPtr &conn) {
            loop->queueInLoop(std::bind(&TcpConncetion::connectDestroyed, conn));
        }

        void removeConnector(const ConnectorPtr &connector) {
            // connector->
        }

    }  // namespace detail

}  // namespace network

TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr,
                     const std::string &nameArg)
    : loop_(loop),
      connector_(new Connector(loop, serverAddr)),
      name_(nameArg),
      connectionCallback_(defaultConnectionCallback),
      messageCallback_(defaultMessageCallback),
      retry_(false),
      connect_(true),
      nextConnId_(1) {
    connector_->setNewConnectionCallback(
            std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
    // FIXME setConnectFailedCallback

}

TcpClient::~TcpClient() {

    TcpConncetionPtr conn;
    bool unique = false;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn) {
        assert(loop_ == conn->getLoop());
        // FIXME: not 100% safe, if we are in different thread
        CloseCallBack cb = std::bind(&detail::removeConnection, loop_, std::placeholders::_1);
        loop_->runInLoop(std::bind(&TcpConncetion::setCloseCallBack, conn, cb));
        if (unique) {
            conn->forceClose();
        }
    } else {
        connector_->stop();
        // FIXME: HACK
        // loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    }
}
/*
 * connect -> start
 * */
void TcpClient::connect() {
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect() {
    connect_ = false;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (connection_) {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop() {
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd) {
    InetAddress peerAddr(sockets::getLocalAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.to_ip_port().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConncetionPtr conn(
            new TcpConncetion(loop_, connName, sockfd, localAddr, peerAddr));

    conn->setConnectionCallBack(connectionCallback_);
    conn->setMessageCallBack(messageCallback_);
    conn->setWriteCompleteCallBack(writeCompleteCallback_);
    conn->setCloseCallBack(
            std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));  // FIXME: unsafe
    {
        std::unique_lock<std::mutex> lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablish();
}

void TcpClient::removeConnection(const TcpConncetionPtr &conn) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConncetion::connectDestroyed, conn));
    if (retry_ && connect_) {
        connector_->restart();
    }
}

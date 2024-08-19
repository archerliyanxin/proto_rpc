#include "Connector.h"
#include <errno.h>

#include "Channel.h"
#include "EventLoop.h"
#include "SocketsOps.h"

using namespace network;
namespace network {
    const int Connector::kMaxRetryDelayMs;

    Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
        : loop_(loop),
          serverAddr_(serverAddr),
          connect_(false),
          state_(kDisconnected),
          retryDelayMs_(kInitRetryDelayMs) {

    }

    Connector::~Connector() {
    }

    void Connector::start() {
        connect_ = true;
        loop_->runInLoop(std::bind(&Connector::startInLoop, this));  // FIXME: unsafe
    }

    void Connector::startInLoop() {
        if (connect_) {
            connect();
        } else {
        }
    }

    void Connector::stop() {
        connect_ = false;
        loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));  // FIXME: unsafe
                                                                    // FIXME: cancel timer
    }

    void Connector::stopInLoop() {
        if (state_ == kConnecting) {
            setState(kDisconnected);
            int sockfd = removeAndResetChannel();
            retry(sockfd);
        }
    }

    void Connector::connect() {
        int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
        int ret = ::connect(sockfd, reinterpret_cast<const sockaddr *>(serverAddr_.get_socketAddr()), static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
        int savedErrno = (ret == 0) ? 0 : errno;
        switch (savedErrno) {
            case 0:
            case EINPROGRESS:
            case EINTR:
            case EISCONN:
                connecting(sockfd);
                break;

            case EAGAIN:
            case EADDRINUSE:
            case EADDRNOTAVAIL:
            case ECONNREFUSED:
            case ENETUNREACH:
                retry(sockfd);
                break;

            case EACCES:
            case EPERM:
            case EAFNOSUPPORT:
            case EALREADY:
            case EBADF:
            case EFAULT:
            case ENOTSOCK:

                ::close(sockfd);
                break;

            default:
                ::close(sockfd);
                // connectErrorCallback_();
                break;
        }
    }

    void Connector::restart() {
        setState(kDisconnected);
        retryDelayMs_ = kInitRetryDelayMs;
        connect_ = true;
        startInLoop();
    }

    void Connector::connecting(int sockfd) {
        setState(kConnecting);
        channel_.reset(new Channel(loop_, sockfd));
        channel_->setWriteCallback(
                std::bind(&Connector::handleWrite, this));  // FIXME: unsafe
        channel_->setErrorCallback(
                std::bind(&Connector::handleError, this));  // FIXME: unsafe

        // channel_->tie(shared_from_this()); is not working,
        // as channel_ is not managed by shared_ptr
        channel_->enableWriting();
    }

    int Connector::removeAndResetChannel() {
        channel_->disableAll();
        channel_->remove();
        int sockfd = channel_->fd();
        // Can't reset channel_ here, because we are inside Channel::handleEvent
        loop_->queueInLoop(
                std::bind(&Connector::resetChannel, this));  // FIXME: unsafe
        return sockfd;
    }

    void Connector::resetChannel() { channel_.reset(); }

    void Connector::handleWrite() {

        if (state_ == kConnecting) {
            int sockfd = removeAndResetChannel();
            int err = sockets::getSocketError(sockfd);
            if (err) {
                retry(sockfd);
            } else if (sockets::isSelfConnect(sockfd)) {
                retry(sockfd);
            } else {
                setState(kConnected);
                if (connect_) {
                    newConnectionCallback_(sockfd);
                } else {
                    ::close(sockfd);
                }
            }
        } else {

        }
    }

    void Connector::handleError() {
        if (state_ == kConnecting) {
            int sockfd = removeAndResetChannel();
            int err = sockets::getSocketError(sockfd);
            retry(sockfd);
        }
    }

    void Connector::retry(int sockfd) {
        ::close(sockfd);
        setState(kDisconnected);
        if (connect_) {

            // loop_->runAfter(retryDelayMs_ / 1000.0,
            //                 std::bind(&Connector::startInLoop, shared_from_this()));
            // retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
        } else {

        }
    }
}  // namespace network
#include "TcpConncetion.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
namespace network{
    void defaultConnectionCallback(const TcpConncetionPtr &conn) {

        // do not call conn->forceClose(), because some users want to register message
        // callback only.
    }

    void defaultMessageCallback(const TcpConncetionPtr &, Buffer *buf) {
        buf->retrieveAll();
    }

    static EventLoop *CheckNULL(EventLoop *loop){
        if(nullptr == loop){
            exit(-1);
        }
        return loop;
    }

    void TcpConncetion::forceCloseInLoop() {
        if (state_ == kConnected || state_ == kDisconnecting) {
            // as if we received 0 byte in handleRead();
            handleClose();
        }
    }

    void TcpConncetion::forceClose() {
        // FIXME: use compare and swap
        if (state_ == kConnected || state_ == kDisconnecting) {
            setState(kDisconnecting);
            loop_->queueInLoop(
                    std::bind(&TcpConncetion::forceCloseInLoop, shared_from_this()));
        }
    }
    TcpConncetion::~TcpConncetion(){

    }
    TcpConncetion::TcpConncetion(EventLoop *loop, const std::string &name,
                  int sockfd,
                  const InetAddress &localAddr,
                  const InetAddress &perAddr)
    : loop_(CheckNULL(loop))
    , name_(name)
    , state_(kConnecting)
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
        if(channel_->isReading()){
            int saveErrno = 0;
            ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
            if(n > 0){
                messageCallBack_(shared_from_this(), &inputBuffer_);
            } else{
                errno = saveErrno;
                handleError();
            }
        }
    }
    void TcpConncetion::handleWrite()
    {
        if(channel_->isWriting()){
            int savaErrno = 0;
            ssize_t n = outputBuffer_.wrieFd(channel_->fd(), &savaErrno);
            if(n > 0){
                outputBuffer_.retrieve(n);
                if(outputBuffer_.readableBytes() == 0){
                    channel_->disableWriting();
                    if(writeCompleteCallBack_){
                        loop_->queueInLoop(std::bind(&TcpConncetion::writeCompleteCallBack_, shared_from_this()));
                    }
                    if(state_ == kDisconnecting){
                        shutdownInLoop();
                    }
                }
            } else{
                network::log_msg("handleWrite error");
            }
        }
    }
    void TcpConncetion::handleClose()
    {
        setState(kDisconnected);
        channel_->disableAll();

        TcpConncetionPtr conPtr(shared_from_this());
        connectionCallBack_(conPtr); // 连接关闭的回调
        closeCallBack_(conPtr); //关闭连接的回调
    }
    void TcpConncetion::handleError()
    {
        int optVal = 0;
        socklen_t socklen = sizeof optVal;
        int err = 0;
        if(::getsockopt(channel_->fd(),SOL_SOCKET, SO_ERROR, &optVal, &socklen) < 0 ){
            err = errno;
        }else{
            err = optVal;
        }
        network::log_msg("TcpConncetion::handleError , name is %s, errno is %d\n", name_.c_str(), err);
    }

    void TcpConncetion::send(Buffer *buf) {
        if (state_ == kConnected) {
            if (loop_->is_in_loopThread()) {
                sendInLoop(buf->peek(), buf->readableBytes());
                buf->retrieveAll();
            } else {
                std::function<void(const std::string &)> fp =
                        [this](const std::string &str) { this->sendInLoop(str); };

                loop_->runInLoop(std::bind(fp, buf->retrieveAllAsString()));
            }
        }
    }

    void TcpConncetion::send(const std::string &buf){
        if(state_ == kConnected){
            if(loop_->is_in_loopThread()){
                sendInLoop(buf);
            } else{
                std::function<void(const std::string &)> fp =
                        [this](const std::string &str) { this->sendInLoop(str); };
                loop_->runInLoop(std::bind(fp, buf));
            }
        }
    }

    void TcpConncetion::sendInLoop(const std::string &message) {
        sendInLoop(message.data(), message.size());
    }

    void TcpConncetion::sendInLoop(const void *data, size_t len) {
        ssize_t nwrote = 0;
        size_t remaining = len;
        bool faultError = false;
        if (state_ == kDisconnected) {
            return;
        }
        // if no thing in output queue, try writing directly
        if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
            nwrote = ::write(channel_->fd(), data, len);
            if (nwrote >= 0) {
                remaining = len - nwrote;
                if (remaining == 0 && writeCompleteCallBack_) {
                    loop_->queueInLoop(
                            std::bind(writeCompleteCallBack_, shared_from_this()));
                }
            } else  // nwrote < 0
            {
                nwrote = 0;
                if (errno != EWOULDBLOCK) {
                    if (errno == EPIPE || errno == ECONNRESET)  // FIXME: any others?
                    {
                        faultError = true;
                    }
                }
            }
        }

        if (!faultError && remaining > 0) {
            size_t oldLen = outputBuffer_.readableBytes();
            outputBuffer_.append(static_cast<const char *>(data) + nwrote, remaining);
            if (!channel_->isWriting()) {
                channel_->enableWriting();
            }
        }
    }



    void TcpConncetion::connectEstablish(){
        setState(kConnected);
        channel_->tie(shared_from_this());
        channel_->enableReading();

        //新连接建立，执行回调
        connectionCallBack_(shared_from_this());
    }

    void TcpConncetion::connectDestroyed(){
        if(state_ == kConnected){
            setState(kDisconnected);
            channel_->disableAll();
            connectionCallBack_(shared_from_this());
        }
        channel_->remove();
    }

    void TcpConncetion::shutdown(){
        if(kConnected == state_ ){
            setState(kDisconnecting);
            loop_->runInLoop([this] { shutdownInLoop(); }); //std::bind(&TcpConncetion::shutdownInLoop, this)
        }
    }

    void TcpConncetion::shutdownInLoop(){
        if(!channel_->isWriting()){
            socket_->shutDownWrite();
        }
    }
} 
#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "sys/eventfd.h"
#include "unistd.h"
#include "iostream"

namespace network{
    //防止一个线程创建多个eventloop
    static thread_local EventLoop *t_eventLoop_thread = nullptr;
    const int kPollTimeMs = 1000;

    // 创建wake_up fd用于创建subReactor 处理新来的channel
    int createEventFd(){
        int event_fd = ::eventfd(0, EFD_CLOEXEC| EFD_NONBLOCK);
        if(event_fd < 0){
            std::cout<<"event_fd error "<<errno<<std::endl;
            ::exit(-1);
        }
        return event_fd;
    }

    EventLoop::EventLoop()
    : loop_(false)
    , quit_(false)
    , thread_id(cur_thread::get_thread_id())
    , poller_(Poller::new_DefaultChannel(this))
    , wakeup_fd(createEventFd())
    , wakeup_channel_(new Channel(this,wakeup_fd))
    , active_channels(0)
    {
        if(t_eventLoop_thread){
            ::exit(-1);
        }else{
            t_eventLoop_thread = this;
        }
        // 设置wakeup感兴趣的事件类型以及发生事件后的回调函数
        wakeup_channel_->setReadCallback(std::bind(&EventLoop::handle_read, this));
        // 每一个eventLoop 都将监听wakeup_channel的epollIn
        wakeup_channel_->enableReading();
    }

    void EventLoop::handle_read() {
        uint64_t notify = 0;
        ssize_t n = ::read(wakeup_fd, static_cast<void*>(&notify), sizeof notify);
        if( (sizeof notify) != n){
            cur_thread::log_msg("handle_read read error %s ,read %d\n", __FUNCTION__, n );
        }
    }

    EventLoop::~EventLoop(){
        wakeup_channel_->disableAll();
        wakeup_channel_->remove();
        ::close(wakeup_fd);
        t_eventLoop_thread = nullptr;
    }

    void EventLoop::updateChannel(network::Channel *channel) {
        poller_->updateChannel(channel);
    }

    void EventLoop::removeChannel(network::Channel *channel) {
        poller_->removeChannel(channel);
    }
    bool EventLoop::hasChannel(network::Channel *channel) {
        poller_->hasChannel(channel);
    }

    // 调动底层poller事件分发器, 监听事件
    void EventLoop::loop() {
        loop_ = true;
        quit_ = false;

        while(!quit_){
            active_channels.clear();
            poller_->poll(kPollTimeMs, &active_channels);

            for (Channel *channel: active_channels) {
                channel->handleEvent();
            }
            doPendingFunctor();
        }

        loop_ = false;
    }

    void EventLoop::quit() {
        loop_ = true;

        if(!is_in_loopThread()){
            wakeup();
        }
    }

    void EventLoop::wakeup() {
        uint64_t one = 1;
        ssize_t n = write(wakeup_fd,&one,sizeof one);
        if(sizeof one == n){
            cur_thread::log_msg("%s error , write wakeup_fd is %d, error code is",__FUNCTION__ ,n, errno);
        }
    }

    void EventLoop::doPendingFunctor() {
        std::vector<Functor> functors;
        calling_pending_functors_ = true;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            functors.swap(pending_functor_);
        }
        for (const Functor &functor: functors) {
            functor();
        }
        calling_pending_functors_ = false;
    }

    void EventLoop::runInLoop(Functor cb) {
        if(is_in_loopThread()){
            cb();
        }else{
            queueInLoop(std::move(cb) );
        }
    }

    void EventLoop::queueInLoop(Functor cb) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            pending_functor_.emplace_back(cb);
        }

        if(!is_in_loopThread() || calling_pending_functors_){
            wakeup();
        }

    }
}
#include "EventLoopThread.h"
#include "EventLoop.h"
namespace network{
    EventLoopThread::EventLoopThread(const ThreadInitBack &cb,
                    const std::string &name)
    : loop_(nullptr)
    , exit_(false)
    , mutex_()
    , cb_(cb){}

    EventLoopThread::~EventLoopThread(){
        exit_ = true;
        if(loop_ != NULL){
            loop_->quit();
            thread_ ->join();
        }
    }

    EventLoop* EventLoopThread::startLoop(){
        thread_ = std::make_unique<std::thread>(std::bind(&EventLoopThread::Thread_func, this));
        EventLoop *eventLoop = nullptr;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(nullptr == loop_ ){
                cv_.wait(lock);
            }
            eventLoop =loop_;
        }
        return eventLoop;
    }

    void EventLoopThread::Thread_func(){
        EventLoop eventLoop;

        if(cb_){
            cb_(&eventLoop);
        }

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.notify_all();
        }
        loop_->loop();
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }
}
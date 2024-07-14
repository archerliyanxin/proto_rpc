#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
namespace network{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg)
    : baseLoop_(baseLoop)
    , name_(nameArg)
    , next_(0)
    , start_(false)
    , threadNum_(0)
    {

    }

    EventLoopThreadPool::~EventLoopThreadPool()
    {

    }

    void EventLoopThreadPool::start(const ThreadInitBack &cb )
    {
        start_ = true;
        for (int i = 0; i < threadNum_; ++i) {
            char buf[name_.size() + 32];
            snprintf(buf,sizeof buf, "%s %d",name_.c_str(),i);
            EventLoopThread *t = new EventLoopThread(cb, buf);
            threads_.push_back(std::unique_ptr<EventLoopThread>(t));
            loops_.push_back(t->startLoop());
        }
        if(0 == threadNum_ && cb){
            cb(baseLoop_);
        }
    }

    EventLoop* EventLoopThreadPool::getNextLoop(){
        EventLoop *loop = baseLoop_;
        if(!loops_.empty()){
            loop = loops_[next_];
            ++next_;
            if(static_cast<size_t>(next_)>loops_.size()){
                next_ = 0;
            }
        }
        return loop;
    }
    std::vector<EventLoop*> EventLoopThreadPool::getAllLoop(){
        if(loops_.empty()){
            return std::vector<EventLoop * >(1, baseLoop_);
        } else{
            return loops_;
        }
    }
}

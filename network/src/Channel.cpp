#include "Channel.h"
#include "EventLoop.h"
namespace network{
    Channel::Channel(network::EventLoop *eventLoop, int fd)
        : loop_(eventLoop),
          fd_(fd),
          events_(0),
          revents_(0),
          index_(-1),
          event_handling_(false),
          addToLoop_(false){}
    Channel::~Channel(){}
    /*
     * 当channel更改fd的events事件的时候，update负责再poller里面更改fd相应事件的epoll_ctl
     * 在channel所属的eventloop中，调用poller方法，注册fd的events事件
     * */
    void Channel::update() {
        loop_->updateChannel(this);
    }
    /*
     * 在chanel所属的EventLoop中，把当前的channel删除掉
     *
     * */
    void Channel::remove(){
        loop_->removeChannel(this);
    }
    // 不知道什么时候被调用过
    void Channel::tie(const std::shared_ptr<void> &obj) {
        tie_ = obj;
        tied_ = true;
    }
    void Channel::handleEvent() {
        event_handling_ = true;
        if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
            if(closeCallBack_){
                closeCallBack_();
            }
        }
        if(revents_ & EPOLLERR){
            if(errorCallBack_){
                errorCallBack_();
            }
        }
        if(revents_ & (EPOLLIN ||EPOLLPRI)){
            if(readCallback_){
                readCallback_();
            }
        }
        if(revents_ & EPOLLOUT){
            if(writeCallBack_){
                writeCallBack_();
            }
        }
    }
}
#pragma once

#include "functional"
#include "memory"
#include "atomic"
#include "sys/epoll.h"
namespace network{
class InetAddress;
class EventLoop;
class Channel{
public:
    enum EventType{
        NoneType = 0,
        ReadType = EPOLLIN || EPOLLPRI,
        WriteType = EPOLLOUT,
    };
    using EventCallBack = std::function<void()>;
    Channel(EventLoop *eventLoop, int fd);
    ~Channel();
    // fd得到poller通知以后，根据具体发生的事件，调用相应的回调
    void handleEvent();
    int fd() const {return fd_;}
    int events() const {return events_;}
    void set_revents(int revent) { revents_ = revent;}

    void enableReading(){ events_ |= EventType::ReadType; update();}
    void disableReading(){ events_ &= ~EventType::ReadType;update();}
    void enableWriting(){ events_|= EventType::WriteType;update();}
    void disableWriting(){events_&= ~EventType::WriteType;update();}
    void disableAll(){events_&=EventType::NoneType;update();}
    bool isNoneEvent(){return events_ = EventType::NoneType;}
    bool isWriting(){return events_&EventType::WriteType;}
    bool isReading(){return events_&EventType::ReadType;}

    //业务相关
    int index(){return index_;}
    void set_index(int index){index_ = index;}

    EventLoop* ownrLoop(){return loop_;}
    void remove();
public:

    void setReadCallback(EventCallBack cb){readCallback_ = std::move(cb);}
    void setWriteCallback(EventCallBack cb){writeCallBack_ = std::move(cb);}
    void setCloseCallback(EventCallBack cb){closeCallBack_ = std::move(cb);}
    void setErrorCallback(EventCallBack cb){errorCallBack_ = std::move(cb);}
    // 防止channel 被remove掉，回调还在执行
    void tie(const std::shared_ptr<void> &);
private:
    void update();
private:
//    static const int kNoneEvent;
//    static const int kReadEvent;
//    static const int kWriteEvent;
    int index_;
private:
    EventLoop *loop_; //事件循环
    const int fd_; //poll监听的对象
    int events_;
    int revents_;

    std::weak_ptr<void> tie_; //跨线程监听channel ptr的生存状态 ,使用时可以把weak_ptr转换为share_ptr
    std::atomic<bool> tied_;
    std::atomic<bool> event_handling_;
    std::atomic<bool> addToLoop_;

    EventCallBack readCallback_;
    EventCallBack writeCallBack_;
    EventCallBack closeCallBack_;
    EventCallBack errorCallBack_;


};
}
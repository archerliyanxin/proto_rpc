#pragma once
#include "memory"
#include "functional"
#include "atomic"
#include "vector"
#include "utils.h"
#include "mutex"
namespace network{
    class Channel;
    class Poller;
    class EventLoop{
    public:
        using Functor = std::function<void()>;
        using ChannelList = std::vector<Channel*>;
        EventLoop();
        ~EventLoop();
        void updateChannel(Channel *channel);
        void removeChannel(Channel *channel);
        bool hasChannel(Channel *channel);
        void loop();
        void quit();
        void doPendingFunctor();
        bool is_in_loopThread() const{return thread_id == cur_thread::get_thread_id();};
        //唤醒loop所在的线程
        void wakeup();
        void runInLoop(Functor cb);
        void queueInLoop(Functor cb);
    private:
        void handle_read(); //wake up
    private:
        std::atomic_bool loop_;
        std::atomic_bool quit_;

        const pid_t thread_id;
        std::unique_ptr<Poller> poller_;

        int wakeup_fd;
        std::unique_ptr<Channel> wakeup_channel_;
        
        ChannelList active_channels;

        std::atomic_bool calling_pending_functors_;
        std::vector<Functor> pending_functor_;

        std::mutex mutex_;

    };
}

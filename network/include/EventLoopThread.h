#pragma once
#include "condition_variable"
#include "memory"
#include "mutex"
#include "thread"
#include "functional"
namespace network{
    class EventLoop;
    class EventLoopThread{
    public:
        using ThreadInitBack = std::function<void(EventLoop*)>;
        EventLoopThread(const ThreadInitBack &cb = ThreadInitBack(),
                        const std::string &name = std::string());

        ~EventLoopThread();

        EventLoop* startLoop();
    private:
        void Thread_func();
    private:
        EventLoop *loop_;
        bool exit_;
        std::unique_ptr<std::thread> thread_;
        std::mutex mutex_;
        std::condition_variable cv_;
        ThreadInitBack cb_;
    };
}

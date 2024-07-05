#pragma once
#include "unordered_map"
#include "vector"
namespace network{
    class EventLoop;
    class Channel;
    class Poller {
    public:
        using ChannelList = std::vector<Channel *> ;

        Poller(EventLoop *eventLoop);
        virtual ~Poller() = default;
        // 所有io复用保留统一的接口
        virtual void poll(int timeoutMs, ChannelList *channelList) = 0;

        virtual void updateChannel(Channel *channel) = 0;
        virtual void removeChannel(Channel *channel) = 0;

        bool hasChannel(Channel *channel) const;
        // eventloop通过这个接口获得poller具体实现
        static Poller *new_DefaultChannel(EventLoop *eventLoop);

    protected:
        // int指代fd
        using ChannelMap  = std::unordered_map<int, Channel *>;
        ChannelMap channels_;
    private:
        EventLoop *OwnerLoop_;
    };
}




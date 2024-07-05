#pragma once
//需要知道基类的详细情况
#include "Poller.h"
#include "sys/epoll.h"
#include "vector"
namespace network{
    /*
     * epoll_create()
     * epoll_ctl
     * epoll_wait
     * */
    class EpollPoller :public Poller{
    public:
        EpollPoller(EventLoop *eventLoop);
        //重写基类poller的抽象方法
        ~EpollPoller() override;

        void poll(int timeoutMs, ChannelList *channelList) override;

        void updateChannel(Channel *channel) override;

        void removeChannel(Channel *channel) override;
    private:
        static const int kInitEventListSize = 16;
        // 填写活跃的链接
        void fillActivateChannel(int numEvents, ChannelList *activateChannels) const;
        // 更新channel通道
        void update(int operatorEvent, Channel *channel);

        using EventList = std::vector<epoll_event>;
        int epoll_fd;
        EventList eventList;
    };
}


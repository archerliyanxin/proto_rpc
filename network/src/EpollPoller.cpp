#include "EpollPoller.h"
#include "Channel.h"
#include "errno.h"
#include "iostream"
#include "logging.h"
#include "unistd.h"
#include "cstring"
namespace {
const int kNew = -1;
const int kAdded = 1;
const int kDelete = 2;
}

namespace network{
EpollPoller::EpollPoller(network::EventLoop *eventLoop)
    :Poller(eventLoop)
    , epoll_fd(::epoll_create1(EPOLL_CLOEXEC))
    , eventList(kInitEventListSize){
    if (epoll_fd < 0){
//        ::google::LogMessageFatal("EPOLLER POLLER ERROR");
    }
}
EpollPoller::~EpollPoller(){
    ::close(epoll_fd);
}
void EpollPoller::updateChannel(Channel *channel){
    const int index = channel->index();
    if(::kNew== index || ::kDelete == index ){
        int fd = channel->fd();
        if(index == ::kNew){
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }else{
        int fd = channel->fd();
        (void )fd;
        if(channel->isNoneEvent()){
            channel->set_index(kDelete);
            update(EPOLL_CTL_DEL, channel);
        }else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel *channel){
    int fd = channel->fd();
    int index = channel->index();
    size_t n = channels_.erase(fd);
    (void)n;
    if(index == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

// epoll_ctl操作，add/mod/del
void EpollPoller::update(int operatorEvent, Channel *channel){
    epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;

    if(::epoll_ctl(epoll_fd, operatorEvent, channel->fd(), &event) < 0){
        std::cout<<"change epoll operator"<<std::endl;
    }
}

void EpollPoller::poll(int timeoutMs, network::Poller::ChannelList *channelList) {
    size_t event_num = epoll_wait(epoll_fd, &*eventList.begin(), static_cast<int>(eventList.size()), timeoutMs);
    int saveError = errno;
    if(event_num > 0){
        fillActivateChannel(event_num, channelList);

        if(event_num == eventList.size()){
            eventList.resize(eventList.size() * 2);
        }
    }else if( 0 == event_num){

    } else{
        if(EINTR != saveError){
            errno = saveError;
        }
    }
}

void EpollPoller::fillActivateChannel(int numEvents, network::Poller::ChannelList *activateChannels) const {
    for(int i = 0; i < numEvents; i++){
        Channel *channel = static_cast<Channel*>(eventList[i].data.ptr);
        channel->set_revents(eventList[i].events);
        activateChannels->push_back(channel); // epoll得带所有的返回列表
    }
}

}
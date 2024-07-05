#include "Poller.h"
#include "Channel.h"
namespace network{
Poller::Poller(network::EventLoop *eventLoop) :OwnerLoop_(eventLoop){}

bool Poller::hasChannel(network::Channel *channel) const {
    auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

}
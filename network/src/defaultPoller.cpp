#include "Poller.h"
#include "stdlib.h"
#include "EpollPoller.h"
namespace network{
    Poller *Poller::new_DefaultChannel(network::EventLoop *eventLoop) {
        // poll
        if (::getenv("RPC_USE_POLL")){
            return nullptr;
        }else{
            //epoll
            return new EpollPoller(eventLoop);
        }
    }
}
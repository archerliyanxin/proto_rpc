#include "SocketsOps.h"
namespace network{
    namespace sockets{
        int createNoBlockSocket(sa_family_t family){
            int sockfd = ::socket(family, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP);

            if(sockfd < 0){
                //            LOG(FATAL)<<" createNoBlockSocket die";
            }
            return sockfd;
        }
    }
}

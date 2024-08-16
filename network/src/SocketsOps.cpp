#include "SocketsOps.h"
#include "strings.h"
#include "utils.h"
namespace network{
    namespace sockets{
        int createNoBlockSocket(sa_family_t family){
            int sockfd = ::socket(family, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP);

            if(sockfd < 0){
                //            LOG(FATAL)<<" createNoBlockSocket die";
            }
            return sockfd;
        }

        sockaddr_in getLocalAddr(int sockfd){
            sockaddr_in localAddr;
            bzero(&localAddr ,sizeof localAddr);
            socklen_t socklen = sizeof localAddr;
            if(::getsockname(sockfd, static_cast<sockaddr *>(static_cast<void *>(&localAddr)), &socklen) < 0){
                cur_thread::log_msg("sockets::getLocalAddr error");
            }
            return localAddr;
        }
    }
}

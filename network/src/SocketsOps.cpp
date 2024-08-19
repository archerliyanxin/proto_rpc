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
                network::log_msg("sockets::getLocalAddr error");
            }
            return localAddr;
        }

        int getSocketError(int sockfd) {
            int optval;
            socklen_t optlen = static_cast<socklen_t>(sizeof optval);
            if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
                return errno;
            } else {
                return optval;
            }
        }

        bool isSelfConnect(int sockfd) {
            struct sockaddr_in localaddr = getLocalAddr(sockfd);
            struct sockaddr_in peeraddr = getLocalAddr(sockfd);
            if (localaddr.sin_family == AF_INET) {
                const struct sockaddr_in *laddr4 =
                        reinterpret_cast<struct sockaddr_in *>(&localaddr);
                const struct sockaddr_in *raddr4 =
                        reinterpret_cast<struct sockaddr_in *>(&peeraddr);
                return laddr4->sin_port == raddr4->sin_port &&
                       laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
            } else {
                return false;
            }
        }

        int createNonblockingOrDie(sa_family_t family) {
            int sockfd =
                    ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP); //非阻塞的 TCP 套接字，并设置了 SOCK_CLOEXEC 选项
            if (sockfd < 0) {
            }
            return sockfd;
        }

    }
}

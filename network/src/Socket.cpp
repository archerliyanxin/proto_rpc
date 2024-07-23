#include "unistd.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "Socket.h"
#include "InetAddress.h"
#include "netinet/in.h"
#include "iostream"

namespace network{
    Socket::~Socket(){
        ::close(socket_fd);
    }

    void Socket::setReuseAddr(bool on){
        int optVal =on ? 1:0;
        ::setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&optVal,static_cast<socklen_t>(sizeof optVal));
    }

    void Socket::setReusePort(bool on){
        int optVal = on?1:0;
        int ret = ::setsockopt(socket_fd,SOL_SOCKET,SO_REUSEPORT,&optVal,static_cast<socklen_t>(sizeof optVal));
        if(ret < 0 && on){
//            LOG(ERROR)<<"SO_REUSEPORT";
        }
    }

    void Socket::keepAlive(bool on){
        int opVal = on ? 1 : 0;
        ::setsockopt(socket_fd,SOL_SOCKET,SO_KEEPALIVE,&opVal,static_cast<socklen_t>(sizeof opVal));
    }

    void Socket::listen(){
        if(0 > ::listen(socket_fd, 1024)){}
    }

    int Socket::accept(InetAddress *peerAddr){
        struct sockaddr_in addr;
        bzero(&addr,sizeof addr);
        socklen_t len = static_cast<socklen_t>(sizeof addr);
        int con_fd = ::accept(socket_fd, reinterpret_cast<sockaddr *>(&addr), &len);
        if(0 > con_fd ){}
        if(0 <= con_fd ){
            peerAddr->set_socketAddr(addr);
        }
        return con_fd;
    }
    void Socket::bindAddress(const InetAddress &addr){
        if(0 > ::bind(socket_fd, reinterpret_cast<const sockaddr *>(addr.get_socketAddr()), static_cast<socklen_t>(sizeof( struct sockaddr_in)))){
            exit(-1);
        }
    }
    void Socket::shutDownWrite(){
        if(::shutdown(socket_fd, SHUT_WR) < 0){
            std::cout<<" shutdown error"<<std::endl;
        }
    }

}

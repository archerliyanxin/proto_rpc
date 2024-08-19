#pragma once

#include "unistd.h"
#include "arpa/inet.h"
//#include "glog/logging.h"
#include <fcntl.h>
#include "errno.h"
namespace network{

    namespace sockets{
        int createNoBlockSocket(sa_family_t family);

        sockaddr_in getLocalAddr(int sockfd);
        int getSocketError(int sockfd);
        bool isSelfConnect(int sockfd);
        int createNonblockingOrDie(sa_family_t family);
    }
}

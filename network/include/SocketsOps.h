#include "unistd.h"
#include "arpa/inet.h"
//#include "glog/logging.h"
#include <fcntl.h>
#include "errno.h"
namespace network{

    namespace sockets{
        int createNoBlockSocket(sa_family_t family);
    }
}

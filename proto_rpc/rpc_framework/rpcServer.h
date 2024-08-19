#include "network/include/TcpServer.h"

namespace google{
namespace protobuf{
    class Service;
}
}

namespace network{
    class rpcServer {
    public:
        rpcServer(EventLoop *loop, const InetAddress &listenAddr);
        void setThreadNum(int numThreads){server_.set_thread_num(numThreads);}

        void RegisterService(::google::protobuf::Service *service);
        void start();
    private:
        void onConnection(const TcpConncetionPtr &conn);
        TcpServer server_;
        std::map<std::string, ::google::protobuf::Service *> serviceMap_;
    };
}





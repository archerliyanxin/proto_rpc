
#include "message.pb.h"
#include <iostream>

#include "network/include/TcpServer.h"
#include "rpc_framework/rpcServer.h"
namespace monitor {
    class TestServiceImpl : public TestService {
    public:
//        TestServiceImpl();
//        ~TestServiceImpl();
        virtual void MonitorInfo(::google::protobuf::RpcController* controller,
                                 const ::monitor::TestRequest* request,
                                 ::monitor::TestResponse* response,
                                 ::google::protobuf::Closure* done) {
            std::cout << " req:\n" << request->DebugString();
            response->set_status(true);
            std::string c = "hight_" + std::to_string(request->count());
            response->set_cpu_info(c);
            done->Run();
        }
    };

}  // namespace monitor
using namespace network;
int main(int argc, char *argv[]) {
//    monitor::TestServiceImpl testService;
//    rpc_framework::Rpc_server rpcServer;
//    rpcServer.RegisterService(&testService);
//    rpcServer.start("127.0.0.1", 8896);
//    ::google::protobuf::ShutdownProtobufLibrary();
    EventLoop loop;
    InetAddress listenAddr(9981);
    monitor::TestServiceImpl impl;
    rpcServer server(&loop, listenAddr);
    server.RegisterService(&impl);
    server.start();
    loop.loop();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
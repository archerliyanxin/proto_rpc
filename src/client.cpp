#include "iostream"
#include "message.pb.h"
#include "network/include/TcpClient.h"
#include "proto_rpc/rpc_framework/rpcChannel.h"
#include "proto_rpc/rpc_framework/rpcCodec.h"
#include "proto_rpc/rpc_framework/rpcServer.h"
#include <thread>
using namespace network;
class RpcClient{
public:
    RpcClient(EventLoop *loop, const InetAddress &addr)
        : loop_(loop),
          client_(loop, addr, "rpcClient"),
          channel_(new rpc_channel),
          _stub(channel_.get())
    {
        client_.setConnectionCallback(std::bind(&RpcClient::onConnection, this, std::placeholders::_1));
        client_.setMessageCallback(std::bind(&rpc_channel::onMessage, channel_.get(), std::placeholders::_1,  std::placeholders::_2));
    }

    void SetMonitorInfo(const monitor::TestRequest &request) {
        monitor::TestResponse *response = new monitor::TestResponse();

        _stub.MonitorInfo(NULL, &request, response,
                          NewCallback(this, &RpcClient::closure, response));
    }


    void connect(){client_.connect();}
private:
    void closure(monitor::TestResponse *resp) {

    }

    void onConnection(const TcpConncetionPtr &conn){
        if(conn->connected()){
            channel_->setConnection(conn);
        } else{
            RpcClient::connect();
        }
    }

    EventLoop *loop_;
    TcpClient client_;
    rpcChannelPtr channel_;
    ::monitor::TestService::Stub _stub;
};
int main(int argc, char *argv[]){
//    rpc_framework::rpc_channel rpcChannel;
//    rpcChannel.init("127.0.0.1", 8896);
//
//    monitor::TestRequest request;
//    monitor::TestResponse response;
//    request.set_name("kkkp");
//    request.set_count(1);
//
//    monitor::TestService_Stub stub(&rpcChannel);
//    rpc_framework::Rpc_Controller controller;
//    stub.MonitorInfo(&controller,&request,&response,NULL);
//    std::cout<<response.cpu_info()<<std::endl;
//    ::google::protobuf::ShutdownProtobufLibrary();
    if (argc > 1) {
        EventLoop loop;
        InetAddress serverAddr(9981, argv[1]);

        RpcClient rpcClient(&loop, serverAddr);
        rpcClient.connect();
        std::unique_ptr<std::thread> thread_ = nullptr;
        int count = 0;
        thread_ = std::make_unique<std::thread>([&]() {
            while (true) {
                count++;

                monitor::TestRequest request;
                request.set_name("cpu0");
                request.set_count(count);

                rpcClient.SetMonitorInfo(request);
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        });
        thread_->detach();
        loop.loop();

    } else {
        printf("Usage: %s host_ip\n", argv[0]);
    }
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}

#include "rpc_framework/rpc_channel.hpp"
#include "rpc_framework/rpc_controller.hpp"
#include "iostream"
#include "message.pb.h"
int main(){
    rpc_framework::rpc_channel rpcChannel;
    rpcChannel.init("127.0.0.1", 8896);

    monitor::TestRequest request;
    monitor::TestResponse response;
    request.set_name("kkkp");
    request.set_count(1);

    monitor::TestService_Stub stub(&rpcChannel);
    rpc_framework::Rpc_Controller controller;
    stub.MonitorInfo(&controller,&request,&response,NULL);
    std::cout<<response.cpu_info()<<std::endl;
    ::google::protobuf::ShutdownProtobufLibrary();
    return 0;
}

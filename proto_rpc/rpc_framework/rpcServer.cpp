#include "rpcServer.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
#include "rpcChannel.h"
namespace network{
    rpcServer::rpcServer(EventLoop *loop, const InetAddress &listenAddr)
    : server_(loop, listenAddr, "RpcServer")
    {
        server_.set_connection_callback(std::bind(&rpcServer::onConnection, this, std::placeholders::_1));
    }

    void rpcServer::RegisterService(::google::protobuf::Service *service) {
        const google::protobuf::ServiceDescriptor *desc = service->GetDescriptor();
        serviceMap_[desc->full_name()] = service;
    }

    void rpcServer::start(){
        server_.start();
    }

    void rpcServer::onConnection(const TcpConncetionPtr &conn){
        if(conn->connected()){
            rpcChannelPtr channel(new rpc_channel(conn));
            channel->setServices(&serviceMap_);
            conn->setMessageCallBack(std::bind(&rpc_channel::onMessage, channel.get(), std::placeholders::_1, std::placeholders::_2));
            log_msg("client connect");
        }else{

        }
    }
}

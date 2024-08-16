#include "rpcServer.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
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

    void rpcServer::start(const std::string &ip, const int port){
        server_.start();
    }

    void rpcServer::onConnection(const TcpConncetionPtr &conn){
        if(conn->connected()){

        }else{

        }
    }
}

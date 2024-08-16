#include "rpc_server1.h"
#include "boost/scoped_ptr.hpp"
#include "functional"
#include "rpc.pb.h"
namespace rpc_framework{
    void Rpc_server::RegisterService(::google::protobuf::Service *service) {
        ServiceInfo serviceInfo;
        serviceInfo.service = service;
        serviceInfo.serviceDescriptor = service->GetDescriptor();
        for (int i = 0; i < serviceInfo.serviceDescriptor->method_count(); ++i) {
            serviceInfo.mds[serviceInfo.serviceDescriptor->method(i)->name()] = serviceInfo.serviceDescriptor->method(i);
        }
        _services[serviceInfo.serviceDescriptor->name()] = serviceInfo;
    }
    void Rpc_server::start(const std::string &ip, const int port) {
        boost::asio::io_service _io;
        boost::asio::ip::tcp::acceptor acceptor(_io,boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip),port));
        while(true){
            auto sock = boost::make_shared<boost::asio::ip::tcp::socket>(_io);
            acceptor.accept(*sock);
            std::cout<<"receive from endpoint"<< sock->remote_endpoint().address()<<std::endl;

            char mata_size[sizeof(int)];
            sock->receive(boost::asio::buffer(mata_size));

            int meta_len = *(int *)mata_size;
            std::vector<char> meta_data(meta_len, 0);
            sock->receive(boost::asio::buffer( meta_data));

            rpc::rpcMeta rpcMeta;
            rpcMeta.ParseFromString(std::string(&meta_data[0],meta_len));

            std::vector<char>  req_data(rpcMeta.data_size(),0);
            sock->receive(boost::asio::buffer(req_data));

            dispatch_msg(rpcMeta.service_name(),rpcMeta.method_name(),std::string(&req_data[0],rpcMeta.data_size()), sock);
        };
    }
    void Rpc_server::dispatch_msg(
            const std::string &service_name,
            const std::string &method_name,
            const std::string &serialized_data,
            const boost::shared_ptr<boost::asio::ip::tcp::socket> &sock)
    {
        auto service = _services[service_name].service;
        auto method_Descriptor = _services[service_name].mds[method_name];

        std::cout << "recieve service_name: "<<service_name <<std::endl;
        std::cout << "recieve method_name: "<< method_name <<std::endl;
        std::cout << "recv type:"<<method_Descriptor->input_type()->name() <<std::endl;
        std::cout << "resp type:"<<method_Descriptor->output_type()->name() <<std::endl;

        auto recv_msg = service->GetRequestPrototype(method_Descriptor).New();
        recv_msg->ParseFromString(serialized_data);
        auto resp_msg = service->GetResponsePrototype(method_Descriptor).New();

        Rpc_Controller rpcController;
//        auto msg_filled_sock
//                = std::bind(&Rpc_server::on_msg_call_filled,this, std::placeholders::_1, std::placeholders::_2, sock);
        auto done = ::google::protobuf::NewCallback(this, &Rpc_server::on_msg_call_filled, resp_msg, sock);
        service->CallMethod(method_Descriptor, &rpcController, recv_msg, resp_msg, done);
    }
    void Rpc_server::on_msg_call_filled(
                            ::google::protobuf::Message *resp,
                            const ::boost::shared_ptr<boost::asio::ip::tcp::socket> sock){
//        boost::scoped_ptr<::google::protobuf::Message> rec_msg_guard(req);
        boost::scoped_ptr<::google::protobuf::Message> resp_msg_guard(resp);

        std::string resp_str;
        pack_msg(resp,&resp_str);

        sock->send(boost::asio::buffer(resp_str));
    }
}


#include "google/protobuf/service.h"
#include "google/protobuf/message.h"
#include "boost/asio.hpp"
#include "boost/make_shared.hpp"
#include "boost/shared_ptr.hpp"
#include "rpc_controller.hpp"
namespace rpc_framework{
    class Rpc_server{

    public:
        Rpc_server()=default;
        void RegisterService(::google::protobuf::Service *service);
        void start(const std::string &ip, const int port);
    private:
        struct ServiceInfo{
            ::google::protobuf::Service *service;
            const ::google::protobuf::ServiceDescriptor *serviceDescriptor;
            std::map<std::string,const ::google::protobuf::MethodDescriptor*>mds;

        };
        std::map<std::string, ServiceInfo> _services;

    private:
        void dispatch_msg(
                const std::string &service_name,
                const std::string &method_name,
                const std::string &serialized_data,
                const boost::shared_ptr<boost::asio::ip::tcp::socket> &sock
                );
        void on_msg_call_filled(
                                ::google::protobuf::Message *resp,
                                const ::boost::shared_ptr<boost::asio::ip::tcp::socket> sock);
        void pack_msg(const ::google::protobuf::Message *msg,
                      std::string *serialzied_str){
            int msg_str = msg->ByteSizeLong();
            serialzied_str->assign((const char*)&msg_str, sizeof(int));

            msg->AppendToString(serialzied_str);
        }
    };
}
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

#include "rpc.pb.h"
#include "boost/asio.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"

namespace google{
    namespace protobuf{
        class Message;
        class Service;
    }
}
namespace rpc_framework {

    class rpc_channel: public ::google::protobuf::RpcChannel{
    public:
        void init(const std::string &ip,const int port){
            _io = boost::make_shared<boost::asio::io_service>();
            _sock = boost::make_shared<boost::asio::ip::tcp::socket>(*_io);
            boost::asio::ip::tcp::endpoint ep (boost::asio::ip::address::from_string(ip),port);
            _sock->connect(ep);
        }
        virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                                ::google::protobuf::RpcController* controller, const ::google::protobuf::Message* request,
                                ::google::protobuf::Message* response, ::google::protobuf::Closure* done) override{
            std::string serialzied_data =  request->SerializeAsString();
            rpc::rpcMeta rpc_meta;
            rpc_meta.set_service_name(method->service()->name());
            rpc_meta.set_method_name(method->name());
            rpc_meta.set_data_size(serialzied_data.size());

            std::string serialzied_str = rpc_meta.SerializeAsString();
            int meta_size = serialzied_str.size();
            serialzied_str.insert(0,std::string((const char*)&meta_size, sizeof(int)));
            serialzied_str += serialzied_data;

            _sock->send(boost::asio::buffer(serialzied_str));

            char res_data_size[sizeof(int)];
            _sock->receive(boost::asio::buffer(res_data_size));

            int res_data_len = *(int *)res_data_size;
            std::vector<char> res_meta_data(res_data_len,0);
            _sock->receive(boost::asio::buffer(res_meta_data));

            response->ParseFromString(std::string(&res_meta_data[0],res_data_len));
        }
    private:
        boost::shared_ptr<boost::asio::io_service> _io; //管理异步io
        boost::shared_ptr<boost::asio::ip::tcp::socket> _sock; //创建套接字并管理端口和ip
    };

}

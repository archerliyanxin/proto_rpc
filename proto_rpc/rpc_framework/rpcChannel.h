#include <google/protobuf/service.h>
#include "rpcCodec.h"
namespace google {
    namespace protobuf {

        // Defined in other files.
        class Descriptor;       // descriptor.h
        class ServiceDescriptor;// descriptor.h
        class MethodDescriptor; // descriptor.h
        class Message;          // message.h

        class Closure;

        class RpcController;
        class Service;

    }// namespace protobuf
}
namespace network {

class rpc_channel : public ::google::protobuf::RpcChannel {
public:
    rpc_channel();
    explicit rpc_channel(const TcpConncetionPtr &conn);
    ~rpc_channel() override;
    void setConnection(const TcpConncetionPtr &conn){conn_ = conn;};
    void setServices(const std::map<std::string ,::google::protobuf::Service *> *services){ services_ = services;}
    virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                                    google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                                    google::protobuf::Message* response, google::protobuf::Closure* done) override;
    void onMessage(const TcpConncetionPtr &ptr, Buffer *buf);

private:
    void onRpcMessage(const TcpConncetionPtr &conn, const RpcMessagePtr &message);
    void doneCallBack(::google::protobuf::Message *response, int64_t id);
    void handle_response_msg(const RpcMessagePtr &messagePtr);
    void handle_request_msg(const TcpConncetionPtr &conn, const RpcMessagePtr &messagePtr);
    struct OutStandingCall{
        ::google::protobuf::Message *response;
        ::google::protobuf::Closure *done;
    };


private:
    TcpConncetionPtr conn_;
    const std::map<std::string, ::google::protobuf::Service *> *services_;
    rpcCodec codec_;
    std::atomic<int> id_;
    std::mutex mutex_;
    std::map<int64_t, OutStandingCall> outStandingCall_;
};
using rpcChannelPtr = std::shared_ptr<rpc_channel>;
}
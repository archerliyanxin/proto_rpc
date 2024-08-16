#include <google/protobuf/service.h>

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
    explicit rpc_channel(const );
    virtual virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                                    google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                                    google::protobuf::Message* response, google::protobuf::Closure* done) override;
private:
}
}
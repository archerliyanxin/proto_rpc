#include "rpcChannel.h"

namespace network{
    rpc_channel::rpc_channel()
    : codec_(std::bind(&rpc_channel::onRpcMessage, this, std::placeholders::_1, std::placeholders::_2))
    , services_(NULL)
    {

    }

    rpc_channel::rpc_channel(const TcpConncetionPtr &conn)
        : codec_(std::bind(&rpc_channel::onRpcMessage, this, std::placeholders::_1, std::placeholders::_2))
          , services_(NULL)
          , conn_(conn)
    {

    }

    void rpc_channel::onMessage(const TcpConncetionPtr &ptr, Buffer *buf){
        codec_.onMessage(conn_, buf);
    }

    rpc_channel::~rpc_channel(){
        for (const auto &item : outStandingCall_) {
            OutStandingCall out = item.second;
            delete out.done;
            delete out.response;
        }
    }

    void rpc_channel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                    google::protobuf::Message* response, google::protobuf::Closure* done)
    {
        RpcMessage message;
        message.set_type(REQUEST);
        int64_t id = id_.fetch_add(1) + 1;
        message.set_id(id);
        message.set_service(method->service()->full_name());
        message.set_method(method->name());
        message.set_request(request->SerializeAsString());

        OutStandingCall out = {response, done};
        {
            std::unique_lock<std::mutex>lock(mutex_);
            outStandingCall_[id] = out ;
        }
        codec_.send(conn_, message);
    }

    void rpc_channel::onRpcMessage(const network::TcpConncetionPtr &conn, const network::RpcMessagePtr &message) {
        RpcMessage& rpcMessage = *message;
        if(rpcMessage.type() == REQUEST){
            handle_request_msg(conn_, message);
        } else if(rpcMessage.type() == RESPONSE){
            handle_response_msg(message);
        }
    }
// message 拿到 service和method , message ->request -> callback -> send
    void rpc_channel::handle_request_msg(const network::TcpConncetionPtr &conn, const network::RpcMessagePtr &messagePtr) {
        RpcMessage &message = *messagePtr;
        ErrorCode error = WRONG_PROTO;
        if (services_) {
            std::map<std::string, google::protobuf::Service *>::const_iterator it =
                    services_->find(message.service());
            if (it != services_->end()) {
                google::protobuf::Service *service = it->second;
                const google::protobuf::ServiceDescriptor *desc =
                        service->GetDescriptor();
                const google::protobuf::MethodDescriptor *method =
                        desc->FindMethodByName(message.method());
                if (method) {
                    std::unique_ptr<google::protobuf::Message> request(
                            service->GetRequestPrototype(method).New());
                    if (request->ParseFromString(message.request())) {
                        google::protobuf::Message *response =
                                service->GetResponsePrototype(method).New();
                        // response is deleted in doneCallback
                        int64_t id = message.id();
                        service->CallMethod(
                                method, NULL, request.get(), response,
                                NewCallback(this, &::network::rpc_channel::doneCallBack, response, id));
                        error = NO_ERROR;
                    } else {
                        error = INVALID_REQUEST;
                    }
                } else {
                    error = NO_METHOD;
                }
            } else {
                error = NO_SERVICE;
            }
        } else {
            error = NO_SERVICE;
        }
        if (error != NO_ERROR) {
            RpcMessage response;
            response.set_type(RESPONSE);
            response.set_id(message.id());
            response.set_error(error);
            codec_.send(conn_, response);
        }
    }
// message.id --> out .response -->  message 到 response 到 done run
    void rpc_channel::handle_response_msg(const network::RpcMessagePtr &messagePtr) {
        RpcMessage &message = *messagePtr;
        int64_t id = message.id();

        OutStandingCall out = {NULL, NULL};

        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto it = outStandingCall_.find(id);
            if (it != outStandingCall_.end()) {
                out = it->second;
                outStandingCall_.erase(it);
            }
        }

        if (out.response) {
            std::unique_ptr<google::protobuf::Message> d(out.response);
            if (!message.response().empty()) {
                out.response->ParseFromString(message.response()); //FIXME
            }
            if (out.done) {
                out.done->Run();
            }
        }
    }

    void rpc_channel::doneCallBack(::google::protobuf::Message *response, int64_t id){
        std::unique_ptr<google::protobuf::Message> d(response);
        RpcMessage message;
        message.set_type(RESPONSE);
        message.set_id(id);
        message.set_response(response->SerializeAsString());  // FIXME: error check
        codec_.send(conn_, message);
    }
}
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/callback.h"

namespace rpc_framework{
    class Rpc_Controller :public google::protobuf::RpcController{
    public:
        virtual void Reset(){}
        virtual bool Failed()const{return false;}
        virtual std::string ErrorText()const{return "";}
        virtual void StartCancel(){}
        virtual void SetFailed(const std::string& reason){}
        virtual bool IsCanceled()const{return false;}
        virtual void NotifyOnCancel(::google::protobuf::Closure* callback){}

    };
}

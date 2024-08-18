#pragma once

#include "iostream"
#include "memory"
#include "unistd.h"
#include "rpc.pb.h"

namespace network{

class Buffer;
class TcpConncetion;
using TcpConncetionPtr =std::shared_ptr<TcpConncetion>;
class RpcMessage;
using RpcMessagePtr = std::shared_ptr<RpcMessage>;
extern  const char rpc_tag[];

class rpcCodec{
public:
    const static int kHeaderLen = sizeof (int32_t);
    const static int kChecksumLen = sizeof (int32_t);
    const static int kMaxMessage =64 * 1024 * 1024;
    enum ErrorCode{
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };
    using ProtoBufMessageCallBack_ = std::function<void(const TcpConncetionPtr&, const RpcMessagePtr &)>;
    using MessagePtr = std::shared_ptr<::google::protobuf::Message>;

    explicit rpcCodec(const ProtoBufMessageCallBack_ &messageCallBack)
            :messageCb_(messageCallBack){};
    ~rpcCodec(){}

    void send(const TcpConncetionPtr& conn, const ::google::protobuf::Message & message );
    void onMessage(const TcpConncetionPtr &conn, Buffer *buf);
    bool parseFromBuffer(const void *buf, int len, ::google::protobuf::Message *message);
    void fillEmptyBuffer(Buffer *buf,const ::google::protobuf::Message &message);
    int serializeToBuffer(const google::protobuf::Message &message, Buffer *buf);
    ErrorCode parse(const char *buf, int len, ::google::protobuf::Message *message);

    static int32_t checksum(const void *buf, int len);
    static bool validateCheckSum(const char *buf, int len);
    static int32_t asInt32(const char *buf);

private:
ProtoBufMessageCallBack_ messageCb_;
int kMinMessageLen_ = 4;
const std::string tag_="PROC0";

};

}
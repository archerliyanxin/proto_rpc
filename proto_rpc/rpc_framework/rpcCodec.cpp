#include "rpcCodec.h"
#include "network/include/TcpConncetion.h"
#include "zlib.h"
namespace network{
    void rpcCodec::send(const TcpConncetionPtr& conn, const ::google::protobuf::Message & message ){
        Buffer buf;
        fillEmptyBuffer(&buf, message);
        conn->send(&buf);
    }
    /*
    *  1）len + (readIndex) + tag +messsage + checkSum + (writeableIndex)
     *  先计算校验和再计算把buffer解析出来
     *  处理messagecb，再收回 1）所有的
    * */
    void rpcCodec::onMessage(const network::TcpConncetionPtr &conn, network::Buffer *buf) {

        while (buf->readableBytes() >=
                   static_cast<uint32_t>(kMinMessageLen_ + kHeaderLen)) {
            const int32_t len = buf->peekInt32();
            if (len > kMinMessageLen_ || len < kMinMessageLen_) {
                // errorCallback_(conn, buf, receiveTime, kInvalidLength);
                break;
            } else if (buf->readableBytes() >= size_t(kHeaderLen + len)) {
                // if (rawCb_ && !rawCb_(conn, StringPiece(buf->peek(), kHeaderLen + len),
                //                       receiveTime)) {
                //   buf->retrieve(kHeaderLen + len);
                //   continue;
                // }
                RpcMessagePtr message(new RpcMessage());
                // FIXME: can we move deserialization & callback to other thread?
                ErrorCode errorCode = parse(buf->peek() + kHeaderLen, len, message.get());
                if (errorCode == kNoError) {
                    // FIXME: try { } catch (...) { }
                    messageCb_(conn, message);
                    buf->retrieve(kHeaderLen + len);
                } else {
                    // errorCallback_(conn, buf, receiveTime, errorCode);
                    break;
                }
            } else {
                break;
            }
        }
    }

    void rpcCodec::fillEmptyBuffer(Buffer *buf,
                                        const google::protobuf::Message &message) {
        buf->append(tag_.data(), tag_.size());

        int byte_size = serializeToBuffer(message, buf);

        int32_t checkSum =
                checksum(buf->peek(), static_cast<int>(buf->readableBytes()));
        buf->appendInt32(checkSum);
        (void)byte_size;
        int32_t len =
                socket::hostToNetwork32(static_cast<int32_t>(buf->readableBytes()));
        buf->prepend(&len, sizeof len);
    }

    int rpcCodec::serializeToBuffer(const google::protobuf::Message &message,
                                         Buffer *buf) {
        int byte_size = google::protobuf::internal::ToIntSize(message.ByteSizeLong());
        buf->ensureWriteableBytes(byte_size + kChecksumLen);

        uint8_t *start = reinterpret_cast<uint8_t *>(buf->beginWrite());
        uint8_t *end = message.SerializeWithCachedSizesToArray(start); //
        if (end - start != byte_size) {
        }
        buf->hasWritten(byte_size);
        return byte_size;
    }

    rpcCodec::ErrorCode rpcCodec::parse( // 计算校验和并且message->ParseFromArray()从void *数组获得message
            const char *buf, int len, ::google::protobuf::Message *message) {
        ErrorCode error = kNoError;

        if (validateCheckSum(buf, len)) {
            if (memcmp(buf, tag_.data(), tag_.size()) == 0) {
                // parse from buffer
                const char *data = buf + tag_.size();
                int32_t dataLen = len - kChecksumLen - static_cast<int>(tag_.size());
                if (parseFromBuffer(data, dataLen, message)) {

                    error = kNoError;
                } else {
                    error = kParseError;
                }
            } else {
                error = kUnknownMessageType;
            }
        } else {
            error = kCheckSumError;
        }

        return error;
    }

    bool rpcCodec::parseFromBuffer(const void *buf, int len,
                                        google::protobuf::Message *message) {
        return message->ParseFromArray(buf, len);
    }

    int32_t rpcCodec::asInt32(const char *buf) {
        int32_t be32 = 0;
        ::memcpy(&be32, buf, sizeof(be32));
        return socket::networkToHost32(be32);
    }

    int32_t rpcCodec::checksum(const void *buf, int len) {
        return static_cast<int32_t>(
                ::adler32(1, static_cast<const Bytef *>(buf), len));
    }

    bool rpcCodec::validateCheckSum(const char *buf, int len) {
        // check sum
        int32_t expectedCheckSum = asInt32(buf + len - kChecksumLen);
        int32_t checkSum = checksum(buf, len - kChecksumLen);
        return checkSum == expectedCheckSum;
    }
}
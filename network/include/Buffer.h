#pragma once
#include "vector"
#include "unistd.h"
#include "string"
#include "cassert"
#include "algorithm"
#include "utils.h"
#include "cstring"
namespace network{
class Buffer{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitSize = 1024;

    explicit Buffer(size_t initSize = kInitSize)
        : buffer_(kCheapPrepend + initSize)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend)
    {}

    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }

    size_t writeableBytes () const{
        int k = buffer_.capacity();
        (void)k;
        return buffer_.size() - writerIndex_;
    }

    size_t prePendableBytes() const {
        return readerIndex_;
    }

    void retrieve(size_t len){
        if(len < readableBytes()){
            readerIndex_ += len;
        }else{
            retrieveAll();
        }
    }

    void retrieveAll(){
        readerIndex_ = writerIndex_ = kCheapPrepend;
    }

    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len){
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void prepend(const void * /*restrict*/ data, size_t len) {
        readerIndex_ -= len;
        const char *d = static_cast<const char *>(data);
        std::copy(d, d + len, begin() + readerIndex_);
    }

    int32_t peekInt32() const {
        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return socket::hostToNetwork32(be32);
    }

    void hasWritten(size_t len) {
        writerIndex_ += len;
    }

    void appendInt32(int32_t x) {
        int32_t be32 = socket::hostToNetwork32(x);
        append(&be32, sizeof be32);
    }

// 缓冲区可读数据的起始地址
    const char * peek() const{
        return begin() + readerIndex_;
    }

    char *beginWrite(){
        return begin() + writerIndex_;
    }

    const char *beginWrite() const{
        return begin() + writerIndex_;
    }

    void ensureWriteableBytes(size_t len){
        if(writeableBytes() < len){
            makeSpace(len);
        }
    }
    void append(const void * /*restrict*/ data, size_t len) {
        append(static_cast<const char *>(data), len);
    }

    void append(const char* data, size_t len){
        ensureWriteableBytes(len);
        std::copy(data, data+len, beginWrite());
    }

    ssize_t readFd(int fd, int *saveErrno);

    ssize_t wrieFd(int fd, int *saveErrno);
private:
    char *begin(){
        // iterator operator *()
        return &*buffer_.begin();
    }

    const char *begin() const {
        // iterator operator *()
        return  &*buffer_.begin();
    }

    void makeSpace(size_t len){
        // ausme writeIndex_ not change
        if(writeableBytes() + prePendableBytes() < len + kCheapPrepend){
            buffer_.resize(writerIndex_ + len);
        } else{
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_ , begin() + kCheapPrepend);
            writerIndex_ = kCheapPrepend + readable;
            readerIndex_ = kCheapPrepend;
            assert(readable == readableBytes());
        }
    }



private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};
}

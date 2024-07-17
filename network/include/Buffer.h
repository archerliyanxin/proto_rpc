#pragma once
#include "vector"
#include "unistd.h"
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
        return buffer_.size() - writerIndex_;
    }



private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};
}

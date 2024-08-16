#include "Buffer.h"
#include "sys/uio.h"
#include "iostream"
namespace network{
    ssize_t Buffer::readFd(int fd, int *saveErrno){
        char extbuf[1024*64];

        struct iovec vec[2];
        int writeable = writeableBytes();
        vec[0].iov_base = beginWrite();
        vec[0].iov_len = writeable;

        vec[1].iov_base = extbuf;
        vec[1].iov_len = sizeof extbuf;

        int iovcnt = (writeable < sizeof extbuf) ? 2 : 1;
        ssize_t n = readv(fd,   vec, iovcnt);
        if(n < 0){
            *saveErrno = errno;
        }else if(n < writeable){
            writerIndex_ += n;
        } else{
            writerIndex_ = buffer_.size();
            append(extbuf, n - writeable);
        }
        return n;
    }

    ssize_t Buffer::wrieFd(int fd, int *saveErrno) {
        int n = ::write(fd, peek(), readableBytes());
        if(n < 0){
            std::cout<<" wrieFd fail write"<<std::endl;
            *saveErrno = n;
        }
        return n;
    }
}
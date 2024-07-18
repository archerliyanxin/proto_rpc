#include "Buffer.h"
#include "sys/uio.h"
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
}
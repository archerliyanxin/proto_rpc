#pragma once

namespace network{
class InetAddress;
class Socket{
public:
explicit Socket(int sock_fd):socket_fd(sock_fd){};
~Socket();
int fd() const {return socket_fd;}
void listen();
int accept(InetAddress *peerAddr);
void bindAddress(const InetAddress &addr);
void shutDownWrite();
private:
    const int socket_fd;
};
}

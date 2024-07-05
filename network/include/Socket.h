#pragma once

namespace network{
class InetAddress;
class Socket{
public:
explicit Socket(int sock_fd):socket_fd(sock_fd){};
~Socket();
private:
    const int socket_fd;
};
}

#pragma once
#include "netinet/in.h"
#include "string"
#include "cstring"
namespace network{
    class InetAddress{
    public:
        explicit InetAddress(const std::string &ip = "127.0.0.1", const uint16_t port = 0);
        explicit InetAddress(const sockaddr_in &addr): addr_(addr){};

        std::string get_ip();
        uint16_t get_port();
        std::string get_ip_port();
        void set_socketAddr(const sockaddr_in &addr){ addr_ = addr;}
        const sockaddr_in* get_socketAddr(){return &addr_;};

    private:
        sockaddr_in addr_;
    };
}


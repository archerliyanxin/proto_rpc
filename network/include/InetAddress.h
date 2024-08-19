#pragma once
#include "netinet/in.h"
#include "string"
#include "cstring"
namespace network{
    class InetAddress{
    public:
        explicit InetAddress(const uint16_t port = 0, const std::string &ip = "127.0.0.1");
        explicit InetAddress(const sockaddr_in &addr): addr_(addr){};

        std::string get_ip() const;
        uint16_t get_port() const;
        const std::string to_ip_port () const;
        std::string get_ip_port();
        sa_family_t family() const { return addr_.sin_family; }
        void set_socketAddr(const sockaddr_in &addr){ addr_ = addr;}
        const sockaddr_in* get_socketAddr() const{return &addr_;};

    private:
        sockaddr_in addr_;
    };
}


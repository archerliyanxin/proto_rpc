#include "InetAddress.h"
#include "arpa/inet.h"
const uint8_t buf_size = 64;
using namespace network;
InetAddress::InetAddress(const std::string &ip , const uint16_t port){
    memset(&addr_,0 ,sizeof addr_);
    addr_.sin_family = AF_INET; //ipv4
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}
uint16_t InetAddress::get_port() {
    return ntohs(addr_.sin_port);
}
std::string InetAddress::get_ip() {
    char buf[buf_size] = "";
    inet_ntop(AF_INET,&addr_.sin_addr.s_addr,buf,sizeof buf);
    return buf;
}
std::string InetAddress::get_ip_port() {
    uint16_t port = get_port();
    char buf[buf_size] = "";
    snprintf(buf, buf_size,":%u", port);
    return get_ip() + buf;
}
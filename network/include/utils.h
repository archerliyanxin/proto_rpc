#pragma once
#include "string"
#include "iostream"
#include "endian.h"
namespace network {
    void cache_id();
    int get_thread_id();

    void log_msg(const char *fmt, ...);


    namespace socket{
        inline uint32_t hostToNetwork32(uint32_t host32) { return htobe32(host32); }
        inline uint32_t networkToHost32(uint32_t net32) { return be32toh(net32); }
    }
}


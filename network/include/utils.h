#pragma once
#include "string"
#include "endian.h"
namespace cur_thread{
    class string ;
    void cache_id();
    int get_thread_id();
    template<typename ...Args>
    void log_msg(const char *fmt, Args ...args);
}
namespace network{
    namespace socket{
        inline uint32_t hostToNetwork32(uint32_t host32) { return htobe32(host32); }
        inline uint32_t networkToHost32(uint32_t net32) { return be32toh(net32); }
    }
}


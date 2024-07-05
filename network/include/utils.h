#pragma once
#include "string"
namespace cur_thread{
    class string ;
    void cache_id();
    int get_thread_id();
    template<typename ...Args>
    void log_msg(const char *fmt, Args ...args);
}

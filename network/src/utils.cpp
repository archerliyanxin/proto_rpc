#include "utils.h"
#include "syscall.h"
#include "unistd.h"
#include "sstream"
#include <stdarg.h>
namespace network {
    static thread_local pid_t t_thread_id = 0 ;
    const int BUF_SIZE = 200;
    static std::ostringstream stream;
    void cache_id(){
        if(t_thread_id == 0){
            t_thread_id = static_cast<pid_t>(::syscall(SYS_getpid));
        }
    }

     pid_t get_thread_id(){
        if(__builtin_expect(t_thread_id == 0,0)){
            cache_id();
        }
        return t_thread_id;
    }

    void log_msg(const char *fmt, ...){
        char logBuf[BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        snprintf(logBuf,BUF_SIZE,fmt, args);
        va_end(args);
        stream<< logBuf;

    }


}
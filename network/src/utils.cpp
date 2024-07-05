#include "utils.h"
#include "syscall.h"
#include "unistd.h"
#include "sstream"
namespace cur_thread{
    static thread_local pid_t t_thread_id = 0 ;
    const int BUF_SIZE = 200;
    std::ostringstream stream;
    void cache_id(){
        if(t_thread_id == 0){
            t_thread_id = static_cast<pid_t>(::syscall(SYS_getpid));
        }
    }
    inline pid_t get_thread_id(){
        if(__builtin_expect(t_thread_id == 0,0)){
            cache_id();
        }
        return t_thread_id;
    }
    template<typename ...Args>
    void log_msg(const char *fmt, Args ...args){
        char logBuf[BUF_SIZE];
        snprintf(logBuf,BUF_SIZE,fmt, args...);
        stream<< logBuf;
    }
}
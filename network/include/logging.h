#include "string"
#include "chrono"
#include "cstdio"
#define BUFFER_SIZE 1024
#ifdef RPCDEBUG
#define LOG_DEBUG(logmsgFornat,...) \
    do \
    { \
        auto now = std::chrono::system_clock();\
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);\
        std::tm now_tm;\
        localtime_r(&now_time_t, &now_tm);  \
        char buf[BUFFER_SIZE] = {0};                      \
        ::snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        std::cout <<"[INFO]";\
        std::cout << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") <<":"<< std::string(buf);\
    }while(0)\
#else
#define LOG_DEBUG(logmsgFornat,...)
#endif

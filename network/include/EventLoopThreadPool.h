#include "vector"
#include "string"
#include "functional"
#include "memory"

namespace network{
    class EventLoop;
    class EventLoopThread;
    class EventLoopThreadPool{
    public:
        using ThreadInitBack = std::function<void(EventLoop*)>;
        EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg);
        ~EventLoopThreadPool();
        void setThreadNum(int nums_){threadNum_ = nums_;}
        void start(const ThreadInitBack &cb = ThreadInitBack());

        EventLoop* getNextLoop();
        std::vector<EventLoop* > getAllLoop();

        bool getStart() const{return start_;}
        const std::string& getName() const {return name_;}
    private:
        EventLoop *baseLoop_;

        std::string name_;
        int next_;
        bool start_;

        int threadNum_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop* > loops_;
    };
}

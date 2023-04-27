#include <thread>
typedef void (*Task)(void *);
namespace cts
{
    class Thread
    {
        friend void taskFunc(void *args);

    private:
        std::thread::id id;
        Task task;
        void *args;
        bool isCore;
        bool hasStarted = false;
        bool isStop = false;

    public:
        void start();
        void stop();
    };
}

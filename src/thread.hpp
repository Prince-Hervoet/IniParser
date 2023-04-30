#include <thread>
typedef void (*Task)(void *);

class Thread
{
    friend void taskFunc(void *args);

private:
    std::thread::id id;
    // task
    Task task;
    void *args;
    bool isCore;
    bool hasStarted = false;
    bool isStop = false;

public:
    Thread(bool isCore)
    {
        this->isCore = isCore;
    }
    void start();
    void stop();
};

#include "thread.hpp"

namespace cts
{
    void taskFunc(void *args)
    {
        cts::Thread *tt = (cts::Thread *)args;
        while (tt->task)
        {
            (tt->task)(tt->args);
        }
    }
}

void cts::Thread::start()
{
    if (this->hasStarted)
    {
        return;
    }
    this->hasStarted = true;
    std::thread(taskFunc, this);
}

void cts::Thread::stop()
{
    if (!this->hasStarted)
    {
        return;
    }
    this->isStop = true;
}
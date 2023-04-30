#include "thread.hpp"

void taskFunc(void *args)
{
    Thread *tt = (Thread *)args;
    while (tt->task)
    {
        (tt->task)(tt->args);
    }
}

void Thread::start()
{
    if (this->hasStarted)
    {
        return;
    }
    this->hasStarted = true;
    std::thread(taskFunc, this);
}

void Thread::stop()
{
    if (!this->hasStarted)
    {
        return;
    }
    this->isStop = true;
}
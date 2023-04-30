#include "thread.hpp"
#include "pool.hpp"

void packTaskFunc(void *args)
{
    Thread *tt = (Thread *)args;
    while (tt->tc || (tt->tc = tt->pool->taskQueue->pop(0)))
    {
        tt->tc->run();
    }
}

void Thread::start()
{
    if (this->hasStarted)
    {
        return;
    }
    this->hasStarted = true;
    std::thread t(packTaskFunc, this);
}

void Thread::stop()
{
    if (!this->hasStarted)
    {
        return;
    }
    this->isStop = true;
}
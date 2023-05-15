#include <iostream>
#include "forthread.hpp"
#include "threadpool.hpp"

void Forthread::start()
{
    if (status == THREAD_STATUS_RUNNING)
    {
        return;
    }
    status = THREAD_STATUS_RUNNING;
    std::thread t(Forthread::threadRunFunc, this);
    this->tid = t.get_id();
    t.detach();
}

void Forthread::threadRunFunc(void *args)
{
    Forthread *ft = (Forthread *)args;
    Task *task = ft->task;
    while (task || (task = ft->tp->taskQueue.poll()))
    {
        try
        {
            task->task(task->args);
        }
        catch (const char *e)
        {
            std::cout << e << std::endl;
        }
        if (ft->status != THREAD_STATUS_RUNNING)
        {
            break;
        }
    }
}

Forthread::Forthread()
{
}

Forthread::Forthread(Task *task, bool isCore)
{
    this->task = task;
    this->isCore = isCore;
}
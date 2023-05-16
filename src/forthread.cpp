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
    void *result = nullptr;
    Expection *exp = nullptr;
    while (task || (task = ft->tp->taskQueue.poll()))
    {
        exp = task->exp;
        if (exp)
        {
            exp->mu.lock();
            if (exp->isCancelFlag)
            {
                exp->setStatus(REJECTED);
                delete task;
                exp->mu.unlock();
                continue;
            }
        }
        try
        {
            result = task->task(task->args);
            if (exp)
            {
                exp->result = result;
                exp->setStatus(FINISHED);
                exp->mu.unlock();
            }
        }
        catch (const char *e)
        {
            std::cout << e << std::endl;
            if (exp)
            {
                exp->result = result;
                exp->setStatus(REJECTED);
                exp->mu.unlock();
            }
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
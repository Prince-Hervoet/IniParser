#include <iostream>
#include "forthread.hpp"
#include "threadpool.hpp"
#include "util.hpp"

void Forthread::start()
{
    if (status == THREAD_STATUS_RUNNING)
    {
        return;
    }
    status = THREAD_STATUS_RUNNING;
    std::thread t(Forthread::threadRunFunc, this);
    this->startTimestamp = getNowTimestamp();
    this->tid = t.get_id();
    t.detach();
}

void Forthread::threadRunFunc(void *args)
{
    Forthread *ft = (Forthread *)args;
    Task *task = ft->task;
    void *result = nullptr;
    Expection *exp = nullptr;
    for (;;)
    {
        while (task || (ft->isCore && (task = ft->tp->taskQueue->poll())) || (!ft->isCore && (task = ft->tp->taskQueue->tryPoll())))
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
                    exp = nullptr;
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
                    exp->result = nullptr;
                    exp->setStatus(REJECTED);
                    exp->mu.unlock();
                }
            }
            delete task;
            task = nullptr;
            exp = nullptr;
            if (ft->status != THREAD_STATUS_RUNNING)
            {
                break;
            }
        }
        if (ft->isCore)
        {
            break;
        }
        else
        {
            time_t now = getNowTimestamp();
            if (ft->startTimestamp + ft->tp->timeout <= now)
            {
                break;
            }
        }
    }
    ft->tp->killThread(ft);
}

Forthread::Forthread(ThreadPool *tp)
{
    this->tp = tp;
}

Forthread::Forthread(Task *task, bool isCore, ThreadPool *tp)
{
    this->task = task;
    this->isCore = isCore;
    this->tp = tp;
}
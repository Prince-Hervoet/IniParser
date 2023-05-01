#include "pack_thread.hpp"
#include "pool.hpp"
#include <iostream>

void PackThread::packRunFunc(void *args)
{
    PackThread *pt = (PackThread *)args;
    CommitTask *task = pt->task;
    int waitTime = pt->isCore ? 500 : 0;
    try
    {
        for (; !pt->isStop;)
        {
            while (task || (task = pt->tp->tasks.pop(waitTime)))
            {
                pt->tp->interruptMu.lock();
                pt->status = THREAD_RUNNING;
                pt->tp->interruptMu.unlock();
                pt->freeTime = getNowTimestamp();
                try
                {
                    task->run();
                }
                catch (const char *msg)
                {
                    std::cout << msg << std::endl;
                }
                delete task;
                task = nullptr;
                pt->tp->interruptMu.lock();
                pt->status = THREAD_READY;
                pt->tp->interruptMu.unlock();
            }
            if (!pt->isCore)
            {
                long_time now = getNowTimestamp();
                if (now - pt->freeTime >= pt->tp->liveTime)
                {
                    break;
                }
            }
        }
        ThreadPool *tp = pt->tp;
        tp->clearThread(pt);
    }
    catch (const std::exception &)
    {
    }
}

void PackThread::start()
{
    if (this->started)
    {
        return;
    }
    started = true;
    this->startTimestamp = getNowTimestamp();
    std::thread t(PackThread::packRunFunc, this);
    this->tid = t.get_id();
    t.detach();
}

void PackThread::stop()
{
    if (this->isStop)
    {
        return;
    }
    isStop = true;
}
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

                try
                {
                    task->run();
                }
                catch (const char *msg)
                {
                    std::cout << msg << std::endl;
                }
                std::cout << "asdfasdfasdf" << std::endl;
                delete task;
                task = nullptr;
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
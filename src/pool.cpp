#include "pool.hpp"
#include <iostream>
// std::cout << "asdfasdfasdf" << std::endl;
int ThreadPool::commit(CommitTask *task)
{
    if (!task)
    {
        return -1;
    }
    else if (this->status >= STOP)
    {
        return 0;
    }
    if (this->coreSize < this->coreLimit)
    {

        if (this->addThread(true, task))
        {

            return 1;
        }
        if (this->status >= STOP)
        {
            return 0;
        }
    }
    if (this->addToQueue(task))
    {
        if (this->solveRemain())
        {
            return 0;
        }
        return 1;
    }
    if (this->addThread(false, task))
    {
        return 1;
    }
    this->reject();
    return -1;
}

bool ThreadPool::addThread(bool isCore, CommitTask *task)
{
    if (this->status >= STOP)
    {
        return false;
    }
    PackThread *thread = nullptr;
    if (isCore && this->coreSize < this->coreLimit)
    {
        mu.lock();
        if (this->coreSize < this->coreLimit && this->status < STOP)
        {

            thread = new PackThread(isCore, task, this);
            this->threads.push_back(thread);
            coreSize += 1;
        }
        mu.unlock();
    }

    if (!isCore)
    {
        if (this->size < this->limit)
        {
            mu.lock();
            if (this->size < this->limit && this->status < STOP)
            {
                thread = new PackThread(isCore, task, this);
                this->threads.push_back(thread);
                size += 1;
            }
            mu.unlock();
        }
    }
    if (!thread)
    {
        return false;
    }
    thread->start();
    return true;
}

bool ThreadPool::addToQueue(CommitTask *task)
{
    return this->tasks.add(task, 0);
}

bool ThreadPool::solveRemain()
{
    bool isPoolStop = false;
    mu.lock();
    if (status >= STOP && tasks.getSize() > 0)
    {
        isPoolStop = true;
        this->addThread(false, nullptr);
    }
    mu.unlock();
    return isPoolStop;
}

void clearThread(PackThread *target)
{
}

void ThreadPool::reject()
{
}

void ThreadPool::clearThread(PackThread *target)
{
}
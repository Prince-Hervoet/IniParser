#include "pool.hpp"

int ThreadPool::commit(CommitTask *task)
{
    if (!task)
    {
        return -1;
    }
    else if (this->status.load() >= STOP)
    {
        return 0;
    }
    if (this->coreSize < this->coreLimit)
    {
        if (this->addThread(true, task))
        {
            return 1;
        }
        if (this->status.load() >= STOP)
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
    if (this->status.load() >= STOP)
    {
        return false;
    }
    PackThread *thread = nullptr;
    if (isCore && this->coreSize < this->coreLimit)
    {
        mu.lock();
        if (this->coreSize < this->coreLimit && this->status.load() < STOP)
        {
            thread = new PackThread(isCore, task);
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
            if (this->size < this->limit && this->status.load() < STOP)
            {
                thread = new PackThread(isCore, task);
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
    return true;
}

bool ThreadPool::addToQueue(CommitTask *task)
{
    return this->tasks.add(*task, 0);
}

bool ThreadPool::solveRemain()
{
    bool isStop = false;
    mu.lock();
    if (status.load() >= STOP && tasks.getSize() > 0)
    {
        isStop = true;
        this->addThread(false, nullptr);
    }
    mu.unlock();
    return isStop;
}

void clearThread(PackThread *target)
{
}
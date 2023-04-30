#include "pool.hpp"
#include "thread.hpp"

int ThreadPool::commit(TaskCommit &task)
{
    if (this->coreSize < this->coreLimit)
    {
        if (this->addThread(true, &task))
        {
            return 1;
        }
    }
    if (this->status >= STOP)
    {
        return -1;
    }
    if (this->addTaskToQueue(&task))
    {
        return 1;
    }
    if (this->addThread(false, nullptr))
    {
        return 1;
    }
    return -1;
}

bool ThreadPool::addThread(bool isCore, TaskCommit *task)
{
    Thread *thread = nullptr;
    if (isCore && this->coreSize < this->coreLimit)
    {
        mu.lock();
        if (this->coreSize < this->coreLimit)
        {
            thread = new Thread(true, task);
            threads.push_back(thread);
            this->coreSize++;
            this->size++;
        }
        mu.unlock();
    }
    if (!isCore)
    {
        if (this->size < this->limit)
        {
            mu.lock();
            if (this->size < this->limit)
            {
                thread = new Thread(false, task);
                threads.push_back(thread);
                this->size++;
            }
            mu.unlock();
        }
    }
    if (thread)
    {
        thread->start();
        return true;
    }
    return false;
}

bool ThreadPool::addTaskToQueue(TaskCommit *task)
{
    if (!task)
    {
        return false;
    }
    return true;
}

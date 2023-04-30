#include "pool.hpp"
#include "thread.hpp"

int ThreadPool::commit(Task task)
{
    if (!task)
    {
        return -1;
    }
    if (this->coreSize < this->coreLimit)
    {
        if (this->addThread(true, task))
        {
            return 1;
        }
    }
    if (this->status >= STOP)
    {
        return -1;
    }
    if (this->addTaskToQueue(task))
    {
        return 1;
    }
    if (this->addThread(false, nullptr))
    {
        return 1;
    }
    return -1;
}

bool ThreadPool::addThread(bool isCore, Task task)
{
    if (isCore && this->coreSize < this->coreLimit)
    {
        mu.lock();
        if (this->coreSize < this->coreLimit)
        {
            Thread *thread = new Thread(true);
            threads.push_back(thread);
            this->coreSize++;
            this->size++;
            mu.unlock();
            return true;
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
                Thread *thread = new Thread(false);
                threads.push_back(thread);
                this->size++;
                mu.unlock();
                return true;
            }
            mu.unlock();
        }
    }
    return false;
}

int ThreadPool::addTaskToQueue(Task task)
{
}

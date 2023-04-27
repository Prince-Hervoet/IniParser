#include "pool.hpp"

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
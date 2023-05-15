#include "threadpool.hpp"

ThreadPool::ThreadPool()
{
}

ThreadPool::ThreadPool(int coreLimit, int maxLimit)
{
    if (coreLimit > 0 && maxLimit > 0)
    {
        this->coreLimit = coreLimit;
        this->maxLimit = maxLimit;
    }
}

ThreadPool::ThreadPool(int coreLimit, int maxLimit, int rejectMode)
{
    if (coreLimit > 0 && maxLimit > 0 && rejectMode <= -1 && rejectMode >= -3)
    {
        this->coreLimit = coreLimit;
        this->maxLimit = maxLimit;
        this->rejectMode = rejectMode;
    }
}

int ThreadPool::commit(CommitTask task, void *args)
{
    if (!task)
    {
        return -1;
    }
    Task *taskPack = new Task();
    taskPack->task = task;
    taskPack->args = args;
    if (coreSize < coreLimit)
    {
        if (createWorker(taskPack, true))
        {
            return 1;
        }
        if (status != POOL_STATUS_RUNNING)
        {
            return 0;
        }
    }
    if (addToQueue(taskPack))
    {
        checkStop();
        return 1;
    }
    if (current < maxLimit)
    {
        if (createWorker(taskPack, false))
        {
            return 1;
        }
    }
}

bool ThreadPool::createWorker(Task *task, bool isCore)
{
    bool isOk = false;
    if (isCore && current < coreLimit)
    {
        mu.lock();
        if (status == POOL_STATUS_RUNNING && current < coreLimit)
        {
            Forthread *ft = new Forthread(task, isCore);
            this->threads.push_back(ft);
            current += 1;
            isOk = true;
        }
        mu.unlock();
    }
    if (!isCore && current < maxLimit)
    {
        mu.lock();
        if (status == POOL_STATUS_RUNNING && current < maxLimit)
        {
            Forthread *ft = new Forthread(task, isCore);
            this->threads.push_back(ft);
            current += 1;
            isOk = true;
        }
        mu.unlock();
    }
    return isOk;
}
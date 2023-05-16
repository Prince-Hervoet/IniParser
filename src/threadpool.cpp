#include "threadpool.hpp"

ThreadPool *get_threadpool(int coreLimit, int maxLimit)
{
    ThreadPool *tp = new ThreadPool(coreLimit, maxLimit);
    return tp;
}

ThreadPool *get_threadpool(int coreLimit, int maxLimit, int rejectMode)
{
    ThreadPool *tp = new ThreadPool(coreLimit, maxLimit, rejectMode);
    return tp;
}

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
    taskPack->exp = nullptr;
    if (this->finallyCommit(taskPack))
    {
        return 1;
    }
    delete taskPack;
    return 0;
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
            ft->start();
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
            ft->start();
        }
        mu.unlock();
    }
    return isOk;
}

Expection *ThreadPool::commitGet(CommitTask task, void *args)
{
    if (!task)
    {
        return nullptr;
    }
    Task *taskPack = new Task();
    Expection *exp = new Expection();
    taskPack->task = task;
    taskPack->args = args;
    taskPack->exp = exp;
    if (this->finallyCommit(taskPack))
    {
        return exp;
    }
    delete exp;
    delete taskPack;
    return nullptr;
}

void ThreadPool::commitBatch(std::initializer_list<CommitTask> tasks)
{
}

bool ThreadPool::finallyCommit(Task *taskPack)
{
    if (coreSize < coreLimit)
    {
        if (createWorker(taskPack, true))
        {
            return true;
        }
        if (status != POOL_STATUS_RUNNING)
        {
            return false;
        }
    }
    if (addToQueue(taskPack))
    {
        checkStop();
        return true;
    }
    if (current < maxLimit)
    {
        if (createWorker(taskPack, false))
        {
            return true;
        }
    }
    return false;
}

void *ThreadPool::batchFunc(void *args)
{
}

void ThreadPool::killThread(Forthread *ft)
{
    std::unique_lock<std::mutex> lock(mu);
    for (auto it = threads.begin(); it != threads.end(); it++)
    {
        if (*it == ft)
        {
            delete *it;
            threads.erase(it);
            break;
        }
    }
}
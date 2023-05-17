#include "threadpool.hpp"
#include <iostream>

ThreadPool *get_threadpool(int coreLimit, int maxThreadLimit, int maxTaskLimit)
{
    ThreadPool *tp = new ThreadPool(coreLimit, maxThreadLimit, maxTaskLimit);
    return tp;
}

ThreadPool *get_threadpool(int coreLimit, int maxLimit, int maxTaskLimit, int rejectMode)
{
    ThreadPool *tp = new ThreadPool(coreLimit, maxLimit, maxTaskLimit, rejectMode);
    return tp;
}

ThreadPool::ThreadPool()
{
    this->taskQueue = new SimpleBlockQueue<Task *>(maxTaskLimit);
}

ThreadPool::ThreadPool(int coreLimit, int maxThreadLimit, int maxTaskLimit)
{
    if (coreLimit > 0 && maxThreadLimit > 0 && maxTaskLimit > 0)
    {
        this->coreLimit = coreLimit;
        this->maxThreadLimit = maxThreadLimit;
        this->maxTaskLimit = maxTaskLimit;
        this->taskQueue = new SimpleBlockQueue<Task *>(maxTaskLimit);
    }
}

ThreadPool::ThreadPool(int coreLimit, int maxThreadLimit, int maxTaskLimit, int rejectMode)
{
    if (coreLimit > 0 && maxThreadLimit > 0 && maxTaskLimit > 0 && rejectMode <= -1 && rejectMode >= -3)
    {
        this->coreLimit = coreLimit;
        this->maxThreadLimit = maxThreadLimit;
        this->maxTaskLimit = maxTaskLimit;
        this->rejectMode = rejectMode;
        this->taskQueue = new SimpleBlockQueue<Task *>(maxTaskLimit);
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
            Forthread *ft = new Forthread(task, true, this);
            this->threads.push_back(ft);
            current += 1;
            coreSize += 1;
            isOk = true;
            ft->start();
        }
        mu.unlock();
    }
    if (!isCore && current < maxThreadLimit)
    {
        mu.lock();
        if (status == POOL_STATUS_RUNNING && current < maxThreadLimit)
        {
            Forthread *ft = new Forthread(task, false, this);
            this->threads.push_back(ft);
            current += 1;
            isOk = true;
            ft->start();
        }
        mu.unlock();
    }
    return isOk;
}

Expectation *ThreadPool::commitGet(CommitTask task, void *args)
{
    if (!task)
    {
        return nullptr;
    }
    Task *taskPack = new Task();
    Expectation *exp = new Expectation();
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
    if (status != POOL_STATUS_RUNNING)
    {
        return false;
    }
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
    if (current < maxThreadLimit)
    {
        if (createWorker(taskPack, false))
        {
            return true;
        }
    }
    return reject(taskPack);
}

bool ThreadPool::reject(Task *task)
{
    if (rejectMode == REJECT_MODE_DISCARD)
    {
        if (taskQueue->tryOffer(task))
        {
            return true;
        }
    }
    else if (rejectMode == REJECT_MODE_WAIT)
    {
        taskQueue->offer(task);
        return true;
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

bool ThreadPool::addToQueue(Task *task)
{
    if (rejectMode == REJECT_MODE_DISCARD)
    {
        return taskQueue->tryOffer(task);
    }
    else if (rejectMode == REJECT_MODE_WAIT)
    {
        taskQueue->offer(task);
    }
    return true;
}

void ThreadPool::checkStop()
{
    mu.lock();
    if (status != POOL_STATUS_RUNNING)
    {
        createWorker(nullptr, false);
    }
    mu.unlock();
}
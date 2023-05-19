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

ThreadPool *get_threadpool(int coreLimit, int maxLimit, int maxTaskLimit, int rejectMode, time_t timeout)
{
    ThreadPool *tp = new ThreadPool(coreLimit, maxLimit, maxTaskLimit, rejectMode, timeout);
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

ThreadPool::ThreadPool(int coreLimit, int maxThreadLimit, int maxTaskLimit, int rejectMode, time_t timeout)
{
    if (coreLimit > 0 && maxThreadLimit > 0 && maxTaskLimit > 0 && rejectMode <= -1 && rejectMode >= -3 && timeout > 0)
    {
        this->coreLimit = coreLimit;
        this->maxThreadLimit = maxThreadLimit;
        this->maxTaskLimit = maxTaskLimit;
        this->rejectMode = rejectMode;
        this->timeout = timeout;
        this->taskQueue = new SimpleBlockQueue<Task *>(maxTaskLimit);
    }
}

/**
 * common commit action (No return value is carried)
 */
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

/**
 * create a worker thread
 */
bool ThreadPool::createWorker(Task *task, bool isCore)
{
    bool isOk = false;
    // 采用双重判断锁模式
    if (isCore && current < coreLimit)
    {
        mu.lock();
        if (status == POOL_STATUS_RUNNING && current < coreLimit)
        {
            Forthread *ft = new Forthread(task, true, this);
            this->threads.push_back(ft); // 将线程的引用放到线程池的列表中
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

/**
 * A commit operation with a return value
 */
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

/**
 * 最终会调用这个commit方法
 */
bool ThreadPool::finallyCommit(Task *taskPack)
{
    // 如果线程池停止了，则不能再添加任务
    if (status != POOL_STATUS_RUNNING)
    {
        return false;
    }

    // 如果核心线程数小于规定的最大值，则选择创建核心线程来完成当前任务
    if (coreSize < coreLimit)
    {
        // 进入创建方法
        if (createWorker(taskPack, true))
        {
            return true;
        }

        // 如果创建核心线程失败，则说明线程池被停止了或者因为并发抢不到机会
        if (status != POOL_STATUS_RUNNING)
        {
            return false;
        }
    }

    // 如果添加核心线程失败，而且没判断到线程池停止，就尝试添加到队列
    if (addToQueue(taskPack))
    {
        // 添加成功之后，加锁判断线程池是否停止
        return checkStop(taskPack);
    }

    // 如果当前线程数小于最大线程数
    if (current < maxThreadLimit)
    {
        // 进入创建线程方法
        if (createWorker(taskPack, false))
        {
            return true;
        }
    }

    // 如果走到这里，则说明以上尝试都是失败，则执行拒绝操作
    return reject(taskPack);
}

bool ThreadPool::reject(Task *task)
{
    if (status != THREAD_STATUS_RUNNING)
    {
        return false;
    }
    // 如果是丢弃模式，则再次尝试添加到队列
    if (rejectMode == REJECT_MODE_DISCARD)
    {
        // 添加失败就返回失败，成功就成功
        if (taskQueue->tryOffer(task))
        {
            return checkStop(task);
        }
    }
    // 如果是等待模式，就一直等待到成功插入队列
    else if (rejectMode == REJECT_MODE_WAIT)
    {
        taskQueue->offer(task);
        return true;
    }
    return false;
}

void *ThreadPool::batchFunc(void *args)
{
    BatchTask *tasks = (BatchTask *)(args);
    CommitTask *run = tasks->tasks;
    void **argsRun = tasks->argss;
    for (int i = 0; i < tasks->count; i++)
    {
        try
        {
            run[i](argsRun[i]);
        }
        catch (const char *e)
        {
            std::cout << e << std::endl;
        }
    }
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
    if (status != THREAD_STATUS_RUNNING)
    {
        return false;
    }
    return taskQueue->tryOffer(task);
}

bool ThreadPool::checkStop(Task *task)
{
    std::unique_lock<std::mutex> lock(mu);
    // 如果插入成功之后，线程池就停止了，则创建非核心线程处理剩下的任务
    if (status != POOL_STATUS_RUNNING && this->taskQueue->size() > 0)
    {
        taskQueue->remove(task);
        return false;
    }
    return true;
}

void ThreadPool::stop()
{
    mu.lock();
    this->status = THREAD_STATUS_STOP;

    mu.unlock();
}
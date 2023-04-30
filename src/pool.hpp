#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include "block_queue.hpp"

typedef void (*Task)(void *);

class Thread;
const int INIT = 0;
const int RUNNING = 1;
const int STOP = 2;
const int OVER = 3;

const int REJECT_BLOCK = 10;
const int REJECT_ERROR = 11;
const int REJECT_FORCE = 12;
const int REJECT_LAST = 13;

class TaskCommit
{
public:
    TaskCommit() {}
    virtual void run() = 0;
};

class ThreadPool
{
    friend class Thread;
    friend void packTaskFunc(void *args);

private:
    int limit = 0;
    int size = 0;
    int coreLimit = 0;
    int coreSize = 0;
    int queueLimit = 0;
    int rejectRule = REJECT_BLOCK;
    std::vector<Thread *> threads;
    BlockQueue<TaskCommit> *taskQueue;
    std::mutex mu;
    volatile int status = INIT;

    bool addThread(bool isCore, TaskCommit *task);
    bool addTaskToQueue(TaskCommit *task);

public:
    ThreadPool()
    {
    }

    int getSize()
    {
        return size;
    }

    int getCoreSize()
    {
        return coreSize;
    }

    int getStatus()
    {
        return status;
    }

    ThreadPool *setLimit(int limit)
    {
        this->limit = limit;
        return this;
    }

    ThreadPool *setCoreLimit(int limit)
    {
        this->coreLimit = limit;
        return this;
    }

    ThreadPool *setQueueLimit(int limit)
    {
        this->queueLimit = limit;
        return this;
    }

    ThreadPool *setRejectRule(int rule)
    {
        if (rule < REJECT_BLOCK || rule > REJECT_LAST)
        {
            return this;
        }
        this->rejectRule = rule;
    }

    int commit(TaskCommit &task);
    void stop();
};
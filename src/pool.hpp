#pragma once
#include "pack_thread.hpp"
#include "block_queue.hpp"
#include <vector>
#include <atomic>
#include <mutex>
#include <chrono>

const int RUNNING = 10;
const int STOP = 20;
const int TERMINATION = 30;

typedef long long longtime;
class CommitTask
{
public:
    virtual void run() = 0;
    CommitTask() {}
};

class ThreadPool
{
    friend class PackThread;

private:
    int size = 0;
    int limit = 0;
    int coreSize = 0;
    int coreLimit = 0;
    int liveTime = 0;
    int rejectType = 0;
    int status = RUNNING;
    longtime startTimestamp;
    std::mutex mu;
    std::mutex interruptMu;
    std::vector<PackThread *> threads;
    BlockQueue<CommitTask *> tasks;
    bool addThread(bool isCore, CommitTask *task);
    bool addToQueue(CommitTask *task);
    bool solveRemain();
    void reject();
    void clearThread(PackThread *target);

public:
    int commit(CommitTask *task);
    void stop()
    {
        this->status = STOP;
    }

    ThreadPool()
        : status(RUNNING)
    {
    }

    ThreadPool
        *
        setLimit(int limit)
    {
        this->limit = limit;
        return this;
    }

    ThreadPool *setCoreLimit(int limit)
    {
        this->coreLimit = limit;
        return this;
    }

    ThreadPool *setRejectType(int type)
    {
        this->rejectType = type;
        return this;
    }
};
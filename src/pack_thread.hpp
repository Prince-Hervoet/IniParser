#pragma once
#include "util.hpp"
#include <thread>

#define THREAD_READY 1
#define THREAD_RUNNING 2

class ThreadPool;
class CommitTask;
typedef void (*PackFunc)(void *);

class PackThread
{
private:
    std::thread::id tid;
    bool isCore = false;
    bool started = false;
    long_time startTimestamp;
    long_time freeTime = 0;
    volatile bool isStop = false;
    volatile int status = THREAD_READY;
    ThreadPool *tp;
    CommitTask *task;
    PackFunc packFunc;
    void *args;

public:
    PackThread()
    {
    }

    PackThread(bool isCore)
        : isCore(isCore)
    {
    }

    PackThread(bool isCore, CommitTask *task)
        : isCore(isCore), task(task)
    {
    }

    PackThread(bool isCore, CommitTask *task, ThreadPool *tp)
        : isCore(isCore), task(task), tp(tp)
    {
    }
    void start();
    void stop();
    static void packRunFunc(void *args);
};
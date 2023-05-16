#pragma once
#include <thread>
#include "util.hpp"
#include "expection.hpp"

class ThreadPool;
typedef struct
{
    CommitTask task;
    void *args;
    Expection *exp;
} Task;

class Forthread
{
private:
    std::thread::id tid;
    Task *task;
    ThreadPool *tp;
    bool isCore = false;
    time_t startTimestamp = 0;
    int status = THREAD_STATUS_STOP;
    static void threadRunFunc(void *args);

public:
    Forthread(ThreadPool *tp);
    Forthread(Task *task, bool isCore, ThreadPool *tp);
    void start();
};

#pragma once
#include <thread>
#include "util.hpp"
#include "expection.hpp"

class ThreadPool;

class Forthread
{
private:
    std::thread::id tid;
    Task *task;
    ThreadPool *tp;
    bool isCore = false;
    int status = THREAD_STATUS_STOP;
    static void threadRunFunc(void *args);

public:
    Forthread();
    Forthread(Task *task, bool isCore);
    void start();
};

typedef struct
{
    CommitTask task;
    void *args;
    Expection *exp;
} Task;

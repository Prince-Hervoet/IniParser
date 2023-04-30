#pragma once
#include "pool.hpp"
#include <thread>
void packTaskFunc(void *args);
class Thread
{
    friend void packTaskFunc(void *args);

private:
    std::thread::id id;
    // task
    TaskCommit *tc;
    ThreadPool *pool;
    bool isCore;
    bool hasStarted = false;
    bool isStop = false;

public:
    Thread(bool isCore)
    {
        this->isCore = isCore;
    }

    Thread(bool isCore, TaskCommit *task)
    {
        this->isCore = isCore;
        this->tc = task;
    }
    void start();
    void stop();
};

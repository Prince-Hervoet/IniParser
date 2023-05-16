#pragma once
#include <mutex>
#include <condition_variable>
#include "util.hpp"
#define PENDING -1
#define FINISHED 1
#define REJECTED 0

class Expection
{
    friend class Forthread;

private:
    std::mutex mu;
    std::condition_variable cond;
    volatile int status = PENDING;
    volatile bool isCancelFlag = false;
    void *result;
    void setStatus(int status);

public:
    int isDone();

    bool cancel();

    bool isCancel();

    void *get();

    void *get(time_t time);
};
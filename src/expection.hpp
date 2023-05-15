#pragma once
#include "util.hpp"
#define PENDING 1
#define FINISHED 2
#define REJECTED 3

class Expection
{
private:
    volatile int status = PENDING;
    void *result;
    Callback callback;

public:
    int getStatus()
    {
        return status;
    }

    void setCallback(Callback func)
    {
        this->callback = func;
    }

    void *getResult()
    {
        return result;
    }
};
#include "expectation.hpp"

int Expectation::isDone()
{
    return status;
}

bool Expectation::cancel()
{
    if (mu.try_lock())
    {
        this->isCancelFlag = true;
        mu.unlock();
        return true;
    }
    else
    {
        return false;
    }
}

bool Expectation::isCancel()
{
    return isCancelFlag;
}

void *Expectation::get()
{
    std::unique_lock<std::mutex> lock(mu);
    while (status == PENDING)
    {
        cond.wait(lock);
    }
    return result;
}

void Expectation::setStatus(int status)
{
    this->status = status;
    cond.notify_one();
}

Expectation::Expectation()
{
}

Expectation::~Expectation()
{
}
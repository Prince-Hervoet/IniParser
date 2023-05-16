#include "expection.hpp"

int Expection::isDone()
{
    return status;
}

bool Expection::cancel()
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

bool Expection::isCancel()
{
    return isCancelFlag;
}

void *Expection::get()
{
    std::unique_lock<std::mutex> lock(mu);
    while (status == PENDING)
    {
        cond.wait(lock);
    }
    return result;
}

void Expection::setStatus(int status)
{
    this->status = status;
    cond.notify_one();
}
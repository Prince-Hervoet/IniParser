#pragma once
#include <list>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockQueue
{
private:
    std::list<T> content;
    int size = 0;
    int limit = 1000;
    std::mutex mu;
    std::condition_variable cond;

public:
    BlockQueue()
    {
    }
    BlockQueue(int limit)
    {
        this->limit = limit > 0 ? limit : 500;
    }
    bool add(T &t, int waitTime)
    {
        std::unique_lock<std::mutex> lck(mu);
        while (size == limit)
        {
            if (waitTime < 0)
            {
                cond.wait(lck);
            }
            else if (waitTime > 0)
            {
                cond.wait_for(lck, std::chrono::milliseconds(waitTime));
                if (size == limit)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        content.push_back(t);
        size++;
        cond.notify_one();
        return true;
    }

    T pop(int waitTime)
    {
        std::unique_lock<std::mutex> lck(mu);
        while (size == 0)
        {
            if (waitTime < 0)
            {
                cond.wait(lck);
            }
            else if (waitTime > 0)
            {
                cond.wait_for(lck, std::chrono::milliseconds(waitTime));
                if (size == 0)
                {
                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
        }
        T t = content.front();
        content.pop_front();
        size--;
        cond.notify_one();
        return t;
    }

    int getSize()
    {
        return size;
    }

    void clear()
    {
        mu.lock();
        if (size == 0)
        {
            mu.unlock();
            return;
        }
    }
};
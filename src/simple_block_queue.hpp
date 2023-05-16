#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>

template <typename T>
class SimpleBlockQueue
{
private:
    std::mutex mu;
    std::condition_variable pollCond;
    std::condition_variable offerCond;
    std::queue<T> data;
    int limit = 512;

public:
    SimpleBlockQueue(int limit)
    {
        this->limit = limit;
    }

    void offer(T t)
    {
        std::unique_lock<std::mutex> lock(mu);
        while (data.size() == limit)
        {
            offerCond.wait(lock);
        }
        data.push(t);
        offerCond.notify_one();
    }

    T poll()
    {
        std::unique_lock<std::mutex> lock(mu);
        while (data.size() == 0)
        {
            offerCond.wait(lock);
        }
        T t = data.front();
        data.pop();
        offerCond.notify_one();
        return t;
    }

    bool tryOffer(T t)
    {
        std::unique_lock<std::mutex> lock(mu);
        if (data.size() == limit)
        {
            return false;
        }
        data.push(t);
        offerCond.notify_one();
        return true;
    }

    T tryPoll()
    {
        std::unique_lock<std::mutex> lock(mu);
        if (data.size() == 0)
        {
            return NULL;
        }
        T t = data.front();
        data.pop();
        offerCond.notify_one();
        return t;
    }

    int size()
    {
        return data.size();
    }
};
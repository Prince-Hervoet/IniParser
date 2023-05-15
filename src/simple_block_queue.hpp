#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class SimpleBlockQueue
{
private:
    std::mutex mu;
    std::condition_variable cond;
    std::queue<T> data;
    int limit = 0;

public:
    void offer(T t)
    {
        std::unique_lock<std::mutex> lock(mu);
        while (data.size() == limit)
        {
            cond.wait();
        }
        data.push(t);
        cond.notify_one();
    }

    T poll()
    {
        std::unique_lock<std::mutex> lock(mu);
        while (data.size() == 0)
        {
            cond.wait();
        }
        T t = data.front();
        T ans = data.pop();
        cond.notify_one();
        return ans;
    }

    int size()
    {
        return data.size();
    }
};
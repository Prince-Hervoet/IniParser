#include <deque>
#include "simple_block_queue.hpp"
#include "forthread.hpp"
#include "expection.hpp"

class ThreadPool
{
    friend class Forthread;

private:
    SimpleBlockQueue<Task *> taskQueue;
    std::deque<Forthread *> threads;
    std::mutex mu;
    int coreSize = 0;
    int coreLimit = 50;
    int maxLimit = 300;
    int current = 0;
    volatile int status = POOL_STATUS_INIT;
    int rejectMode = REJECT_MODE_DISCARD;
    bool createWorker(Task *task, bool isCore);
    bool addToQueue(Task *task);
    void checkStop();

public:
    ThreadPool();
    ThreadPool(int coreLimit, int maxLimit);
    ThreadPool(int coreLimit, int maxLimit, int rejectMode);
    int commit(CommitTask task, void *args);
    Expection *commitAndGet(CommitTask task, void *args);
    void stop();
};
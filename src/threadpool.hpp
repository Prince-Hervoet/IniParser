#include <deque>
#include <initializer_list>
#include "simple_block_queue.hpp"
#include "forthread.hpp"
#include "expectation.hpp"

class ThreadPool
{
    friend class Forthread;

private:
    SimpleBlockQueue<Task *> *taskQueue;
    std::deque<Forthread *> threads;
    std::mutex mu;
    int coreSize = 0;
    int coreLimit = 50;
    int maxTaskLimit = 1024;
    int maxThreadLimit = 300;
    int current = 0;
    time_t timeout = 1000;
    volatile int status = POOL_STATUS_RUNNING;
    int rejectMode = REJECT_MODE_DISCARD;
    bool createWorker(Task *task, bool isCore);
    bool finallyCommit(Task *task);
    bool addToQueue(Task *task);
    void checkStop();
    void killThread(Forthread *ft);
    bool reject(Task *task);
    static void *batchFunc(void *args);

public:
    ThreadPool();
    ThreadPool(int coreLimit, int maxThreadLimit, int maxTaskLimit);
    ThreadPool(int coreLimit, int maxThreadLimit, int maxTaskLimit, int rejectMode);
    void setTimeout(time_t time);
    int commit(CommitTask task, void *args);
    Expectation *commitGet(CommitTask task, void *args);
    void commitBatch(CommitTask tasks[], void *argss[], int count);
    void stop();

    int getCurrent()
    {
        return current;
    }

    int getCoreSize()
    {
        return coreSize;
    }

    int getQueueSize()
    {
        return taskQueue->size();
    }
};

ThreadPool *get_threadpool(int coreLimit, int maxThreadLimit, int maxTaskLimit);
ThreadPool *get_threadpool(int coreLimit, int maxThreadLimit, int maxTaskLimit, int rejectMode);
#include "pool.hpp"
#include "test.hpp"
#include <iostream>
#include "block_queue.hpp"

int main()
{
    ThreadPool *tp = new ThreadPool();
    tp->setCoreLimit(100)->setLimit(200);
    CommitTask *t = new Test();
    tp->commit(t);
    // std::cout << "asdfasdfasdf" << std::endl;
    for (;;)
    {
    }

    // BlockQueue<int> *queue = new BlockQueue<int>();
    // int a = 132123;
    // int b = 3434;
    // queue->add(a, 0);
    // queue->add(b, 0);
    // queue->pop(0);
    // queue->pop(0);
    // queue->pop(0);
    // queue->pop(0);
    // std::cout << queue->getSize() << std::endl;
    return 0;
}
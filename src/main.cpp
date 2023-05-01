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
    tp->commit(t);
    // std::cout << "asdfasdfasdf" << std::endl;
    for (;;)
    {
    }
    return 0;
}
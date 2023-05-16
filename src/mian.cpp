#include <iostream>
#include "threadpool.hpp"

void *test(void *args)
{
    std::cout << "操你妈，志超" << std::endl;
}

int main()
{
    ThreadPool *tp = get_threadpool(20, 300);
    tp->commit(test, nullptr);
    for (;;)
    {
    }
}
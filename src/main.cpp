#include <iostream>
#include <chrono>
#include <thread>
#include "threadpool.hpp"

void *test(void *args)
{
    std::cout << "操你妈test" << std::endl;
}

int main()
{
    ThreadPool *tp = get_threadpool(2, 64, 5000);
    tp->commit(test, nullptr);

    for (;;)
    {
    }
}
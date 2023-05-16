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
    ThreadPool *tp = get_threadpool(20, 300, 5000);
    tp->commit(test, nullptr);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    tp->commit(test, nullptr);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    tp->commit(test, nullptr);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << tp->getCoreSize() << std::endl;
    for (;;)
    {
    }
}
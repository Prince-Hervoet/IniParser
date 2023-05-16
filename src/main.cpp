#include <iostream>
#include <chrono>
#include <thread>
#include "threadpool.hpp"

void *test(void *args)
{
    std::cout << "操你妈test" << std::endl;
    int *a = new int(123);
    return a;
}

int main()
{
    ThreadPool *tp = get_threadpool(2, 300, 5000);
    Expection *exp = tp->commitGet(test, nullptr);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << *(int *)(exp->get()) << std::endl;
    for (;;)
    {
    }
}
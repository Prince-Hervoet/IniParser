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
    ThreadPool *tp = get_threadpool(2, 64, 5000);
    Expectation *exp = tp->commitGet(test, nullptr);
    void *result = exp->get();
    std::cout << *((int *)result) << "asdfsadf" << std::endl;
    for (;;)
    {
    }
}
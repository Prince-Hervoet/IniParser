#include "pool.hpp"
#include <iostream>
class Test : public CommitTask
{
public:
    Test() {}
    void run()
    {
        std::cout << "我被执行了。。。" << std::endl;
    }
};
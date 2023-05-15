#include "util.hpp"

long_time getNowTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);
    long long now_ll = static_cast<long long>(now_t);
    return now_ll;
}
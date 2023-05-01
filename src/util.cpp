#include "util.hpp"

long_time getNowTimestamp()
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为 time_t 类型
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);

    // 将 time_t 转换为 long long 类型
    long long now_ll = static_cast<long long>(now_t);
    return now_ll;
}
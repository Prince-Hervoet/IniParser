#pragma once
#include <chrono>

typedef long long long_time;

typedef void *(*CommitTask)(void *);
typedef void (*Callback)(void *);

#define THREAD_STATUS_STOP 1
#define THREAD_STATUS_RUNNING 2

#define REJECT_MODE_DISCARD -1
#define REJECT_MODE_WAIT -2
#define REJECT_MODE_THROW -3

#define POOL_STATUS_INIT 5
#define POOL_STATUS_STOP -10
#define POOL_STATUS_RUNNING 10
#define POOL_STATUS_STOPING 20

long_time getNowTimestamp();
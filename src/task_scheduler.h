// task_scheduler.h
#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <iostream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <algorithm>

class TaskScheduler {
public:
    TaskScheduler();
    ~TaskScheduler();

    void Add(std::function<void()> task, std::time_t timestamp);

private:
    void Worker();

    std::vector<std::pair<std::time_t, std::function<void()>>> tasks;

    std::mutex mtx;                    
    std::condition_variable cv;         
    bool stop;                         
    std::thread worker_thread;         
};

#endif // TASK_SCHEDULER_H

#include "task_scheduler.h"

TaskScheduler::TaskScheduler() : stop(false) {
  worker_thread = std::thread(&TaskScheduler::Worker, this);
}

TaskScheduler::~TaskScheduler() {
  {
    std::unique_lock<std::mutex> lock(mtx);
    stop = true;
  }
  cv.notify_all();
  if (worker_thread.joinable()) {
    worker_thread.join();
  }
}

void TaskScheduler::Add(std::function<void()> task, std::time_t timestamp) {
  std::unique_lock<std::mutex> lock(mtx);

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  if (timestamp < now) {
    throw std::invalid_argument("Timestamp cannot be in the past.");
  }

  std::pair<std::time_t, std::function<void()>> new_task = std::make_pair(timestamp, task);

  auto it = std::find_if(tasks.begin(), tasks.end(), [&](const auto& existing_task) {
    return existing_task.first > timestamp;  
  });

  tasks.insert(it, new_task);

  cv.notify_all();  
}

void TaskScheduler::Worker() {
  while (true) {
    std::function<void()> task;
    std::time_t exec_time;

    {
      std::unique_lock<std::mutex> lock(mtx);

      cv.wait(lock, [this] {
        return stop || !tasks.empty();
      });

      if (stop && tasks.empty()) {
        break;
      }

      time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      if (!tasks.empty() && tasks.front().first <= now) {
        exec_time = tasks.front().first;
        task = tasks.front().second;
        tasks.erase(tasks.begin()); 
      } else if (!tasks.empty()) {
        cv.wait_until(lock, std::chrono::system_clock::from_time_t(tasks.front().first));
        continue;
      }
    }

    if (task) {
      std::thread task_thread(task); 
      task_thread.detach();          
    }
  }
}

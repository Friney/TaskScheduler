#include "../src/task_scheduler.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>

class TaskSchedulerTest : public ::testing::Test {
 protected:
  TaskScheduler scheduler;
};

TEST_F(TaskSchedulerTest, ExecutesSingleTask) {
  std::atomic<bool> taskExecuted{false};

  scheduler.Add(
      [&taskExecuted]() {
        taskExecuted = true;
      },
      std::time(nullptr) + 1);

  std::this_thread::sleep_for(std::chrono::seconds(2));

  EXPECT_TRUE(taskExecuted);
}

TEST_F(TaskSchedulerTest, ExecutesMultipleTasksInOrder) {
  std::vector<int> results;
  scheduler.Add([&results]() { results.push_back(2); }, std::time(nullptr) + 2);
  scheduler.Add([&results]() { results.push_back(1); }, std::time(nullptr) + 1);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  ASSERT_EQ(results.size(), 2);
  EXPECT_EQ(results[0], 1);
  EXPECT_EQ(results[1], 2);
}

TEST_F(TaskSchedulerTest, WrongTimeTask) {
  EXPECT_THROW(scheduler.Add([]() {}, std::time(nullptr) - 1), std::invalid_argument);
}

TEST_F(TaskSchedulerTest, WorkAfterThrow) {
  EXPECT_THROW(scheduler.Add([]() {}, std::time(nullptr) - 1), std::invalid_argument);
  std::atomic<bool> taskExecuted{false};

  scheduler.Add(
      [&taskExecuted]() {
        taskExecuted = true;
      },
      std::time(nullptr) + 1);

  std::this_thread::sleep_for(std::chrono::seconds(2));

  EXPECT_TRUE(taskExecuted);
}

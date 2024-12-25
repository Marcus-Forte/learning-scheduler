#pragma once

#include <functional>
#include <list>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <thread>

using TaskT = std::function<void()>;

/**
 * @brief Handle that signals the scheduler when periodic task should be
 * finished.
 *
 */
struct PeriodicTaskHandle {
  PeriodicTaskHandle();
  void finish();
  bool isFinished() const;

private:
  bool is_finished_;
};

struct PeriodicTask {
  PeriodicTask(TaskT &&fn,
               const std::chrono::duration<unsigned int, std::milli> &duration,
               std::shared_ptr<PeriodicTaskHandle> &handle)
      : fn_(std::move(fn)), duration_(duration), handle_(handle) {}

public:
  TaskT fn_;
  std::chrono::duration<unsigned int, std::milli> duration_;
  std::shared_ptr<PeriodicTaskHandle> handle_;
  std::chrono::time_point<std::chrono::steady_clock> last_call_time_;
};

/**
 * @brief Scheduler class. Creates a dedicated thread to execute tasks.
 *
 */
class Scheduler {
public:
  Scheduler();
  void start();

  /**
   * @brief Schedule task to be run asynchronously.
   *
   * @param fn Task.
   */
  void schedule(TaskT &&fn);

  /**
   * @brief Schedule task to be run at a specified period.
   *
   * @param dur Time in milliseconds
   * @param fn Function to be run.
   * @return std::shared_ptr<PeriodicTaskHandle> handle to be signalled by the
   * client if to unschedule the task.
   */
  std::shared_ptr<PeriodicTaskHandle>
  schedule_periodic(std::chrono::duration<unsigned int, std::milli> dur,
                    TaskT &&fn);

  /**
   * @brief Stop the scheduler
   *
   */
  void stop();

private:
  void main_loop(std::stop_token stop_token);

private:
  std::queue<TaskT> task_list_;
  std::list<PeriodicTask> periodic_task_list_;
  std::jthread main_;
  std::shared_mutex mutex_;
};
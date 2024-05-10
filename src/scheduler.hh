#pragma once

#include <chrono>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <queue>

using TaskT = std::function<void()>;

/**
 * @brief Handle that signals the scheduler when periodic task should be
 * finished.
 *
 */
struct PeriodicTaskHandle {
  void finish() { is_finished_ = true; }
  bool is_finished_ = false;
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

class Scheduler {
public:
  Scheduler();
  ~Scheduler();
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
  void stop();

private:
  void main_loop();

private:
  std::queue<TaskT> task_list_;
  std::list<PeriodicTask> periodic_task_list_;
  std::future<void> main_;
  bool is_running_;
};
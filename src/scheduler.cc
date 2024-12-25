
#include "scheduler.hh"
#include <mutex>

PeriodicTaskHandle::PeriodicTaskHandle() : is_finished_(false) {}
void PeriodicTaskHandle::finish() { is_finished_ = true; }
bool PeriodicTaskHandle::isFinished() const { return is_finished_; }

Scheduler::Scheduler() = default;

void Scheduler::start() { main_ = std::jthread(&Scheduler::main_loop, this); }

void Scheduler::schedule(TaskT &&fn) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  task_list_.push(std::move(fn));
}

std::shared_ptr<PeriodicTaskHandle> Scheduler::schedule_periodic(
    std::chrono::duration<unsigned int, std::milli> dur, TaskT &&fn) {

  auto handle = std::make_shared<PeriodicTaskHandle>();

  std::unique_lock<std::shared_mutex> lock(mutex_);
  periodic_task_list_.push_front({std::move(fn), dur, handle});

  return handle;
}

void Scheduler::stop() { main_.request_stop(); }

void Scheduler::main_loop(std::stop_token stop_token) {

  while (!stop_token.stop_requested()) {

    while (!task_list_.empty()) {
      auto &task = task_list_.front();
      task();
      {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        task_list_.pop();
      }
    }

    auto current_time = std::chrono::steady_clock::now();

    for (auto it = periodic_task_list_.begin(); it != periodic_task_list_.end();
         it++) {

      if (it->handle_->isFinished()) {
        {
          std::unique_lock<std::shared_mutex> lock(mutex_);
          it = periodic_task_list_.erase(it);
        }
        continue;
      }

      const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
          current_time - it->last_call_time_);
      if (delta >= it->duration_) {
        it->fn_();
        it->last_call_time_ = current_time;
      }
    }

    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
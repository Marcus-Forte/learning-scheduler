
#include <chrono>
#include <future>
#include <memory>

#include "scheduler.hh"

Scheduler::Scheduler() : is_running_(false) {}

Scheduler::~Scheduler() { stop(); }
void Scheduler::start() {
  is_running_ = true;
  main_ = std::async(std::launch::async, [this] { main_loop(); });
}

void Scheduler::schedule(TaskT &&fn) { task_list_.push(std::move(fn)); }

std::shared_ptr<PeriodicTaskHandle> Scheduler::schedule_periodic(
    std::chrono::duration<unsigned int, std::milli> dur, TaskT &&fn) {

  auto handle = std::make_shared<PeriodicTaskHandle>();
  periodic_task_list_.push_front({std::move(fn), dur, handle});

  return handle;
}

void Scheduler::stop() {
  is_running_ = false;
  main_.get();
}

void Scheduler::main_loop() {

  while (is_running_) {

    while (!task_list_.empty()) {
      auto &task = task_list_.front();
      task();
      task_list_.pop();
    }

    auto current_time = std::chrono::steady_clock::now();

    for (auto it = periodic_task_list_.begin(); it != periodic_task_list_.end();
         it++) {

      if (it->handle_->is_finished_) {
        it = periodic_task_list_.erase(it);
        continue;
      }

      auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
          current_time - it->last_call_time_);
      if (delta >= it->duration_) {
        it->fn_();
        it->last_call_time_ = current_time;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
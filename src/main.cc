#include <chrono>
#include <iostream>
#include <memory>

#include "scheduler.hh"

void periodic_task() { std::cout << "Calling every... 200ms" << std::endl; }

class Consumer {
public:
  Consumer(const std::shared_ptr<Scheduler> &scheduler)
      : scheduler_(scheduler) {}

  ~Consumer() {}
  void process() {
    scheduler_->schedule([this] { data_.push_back(5); });
    scheduler_->schedule([this] { data_.push_back(6); });
    task_ =
        scheduler_->schedule_periodic(std::chrono::milliseconds(50), [this] {
          data_.push_back(data_.size());
        });
  }

  void print() {
    for (const auto &el : data_) {
      std::cout << el << std::endl;
    }
  }

private:
  std::shared_ptr<Scheduler> scheduler_;
  std::shared_ptr<PeriodicTaskHandle> task_;
  std::vector<double> data_;
};

int main() {
  auto scheduler = std::make_shared<Scheduler>();

  scheduler->start();
  Consumer consumer(scheduler);

  consumer.process();

  auto handle1 = scheduler->schedule_periodic(std::chrono::milliseconds(200),
                                              periodic_task);

  auto handle2 =
      scheduler->schedule_periodic(std::chrono::milliseconds(100), [] {
        std::cout << "calling every 100ms..." << std::endl;
      });

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  handle1->finish();
  handle2->finish();

  std::cout << "Finished" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  consumer.print();

  return 0;
}
#include "scheduler.h"

void Scheduler::tick() {
  unsigned long now = millis();
  for (auto& t : tasks) {
    if (now - t.lastRun >= t.interval) {
      t.lastRun = now;
      t.fn();
    }
  }
}

void Scheduler::addTask(unsigned long interval, std::function<void()> fn) {
  tasks.push_back({interval, 0, fn});
}

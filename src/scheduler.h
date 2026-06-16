#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <vector>
#include <functional>
struct Task {
  unsigned long interval;
  unsigned long lastRun;
  std::function<void()> fn;
};
class Scheduler {
public:
  void tick();
  void addTask(unsigned long interval, std::function<void()> fn);
private:
  std::vector<Task> tasks;
};
#endif

#ifndef AUTOMATION_H
#define AUTOMATION_H
#include <ArduinoJson.h>
#include <vector>

struct Rule {
  String condition;
  String action;
  unsigned long lastTriggered;
};

class AutomationEngine {
public:
  void evaluate();
  void addRule(const String& cond, const String& act);
  void loadRules();
private:
  std::vector<Rule> rules;
};
#endif

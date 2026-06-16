#include "automation.h"
#include <LittleFS.h>

void AutomationEngine::evaluate() {
  unsigned long now = millis();
  for (auto& r : rules) {
    if (now - r.lastTriggered > 10000) {
      if (r.condition == "always") {
        Serial.println("Executing action: " + r.action);
        r.lastTriggered = now;
      }
    }
  }
}

void AutomationEngine::addRule(const String& cond, const String& act) {
  rules.push_back({cond, act, 0});
}

void AutomationEngine::loadRules() {
  if (LittleFS.exists("/automation.json")) {
    File f = LittleFS.open("/automation.json", "r");
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, f);
    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject o : arr) {
      addRule(o["condition"], o["action"]);
    }
    f.close();
  }
}

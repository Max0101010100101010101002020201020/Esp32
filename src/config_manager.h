#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "wifi_manager.h"

class ConfigManager {
public:
  void loadConfig();
  void saveConfig(const String& json);
  String getConfigJson();
  WiFiConfig getWifiConfig();
private:
  DynamicJsonDocument configDoc{2048};
};
#endif

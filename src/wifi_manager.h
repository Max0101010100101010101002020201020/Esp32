#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include <WiFi.h>
#include <ArduinoJson.h>
#include <vector>

struct WiFiConfig {
  String ssid;
  String password;
  String mode;
  String ap_ssid;
  String ap_password;
};

class WiFiManager {
public:
  void begin(const WiFiConfig& cfg);
  void applyConfig(const WiFiConfig& cfg);
  String scanNetworks();
  WiFiConfig& getConfig();
private:
  WiFiConfig config;
  unsigned long lastScan = 0;
};
#endif

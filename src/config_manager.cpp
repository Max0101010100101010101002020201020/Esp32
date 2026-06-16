#include "config_manager.h"

void ConfigManager::loadConfig() {
  if (LittleFS.exists("/config.json")) {
    File f = LittleFS.open("/config.json", "r");
    deserializeJson(configDoc, f);
    f.close();
  } else {
    configDoc["wifi"]["ssid"] = "";
    configDoc["wifi"]["password"] = "";
    configDoc["wifi"]["mode"] = "AP";
    configDoc["wifi"]["ap_ssid"] = "ESP32-Config";
    configDoc["wifi"]["ap_password"] = "12345678";
    configDoc["security"]["users"][0]["username"] = "admin";
    configDoc["security"]["users"][0]["password"] = "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918";
    String out;
    serializeJson(configDoc, out);
    File f = LittleFS.open("/config.json", "w");
    f.print(out);
    f.close();
  }
}

void ConfigManager::saveConfig(const String& json) {
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, json);
  configDoc = doc;
  File f = LittleFS.open("/config.json", "w");
  serializeJson(configDoc, f);
  f.close();
}

String ConfigManager::getConfigJson() {
  String out;
  serializeJson(configDoc, out);
  return out;
}

WiFiConfig ConfigManager::getWifiConfig() {
  WiFiConfig cfg;
  cfg.ssid = configDoc["wifi"]["ssid"] | "";
  cfg.password = configDoc["wifi"]["password"] | "";
  cfg.mode = configDoc["wifi"]["mode"] | "AP";
  cfg.ap_ssid = configDoc["wifi"]["ap_ssid"] | "ESP32-Config";
  cfg.ap_password = configDoc["wifi"]["ap_password"] | "12345678";
  return cfg;
}

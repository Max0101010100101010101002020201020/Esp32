#include "wifi_manager.h"

void WiFiManager::begin(const WiFiConfig& cfg) {
  config = cfg;
  if (config.mode == "AP" || config.ssid.isEmpty()) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(config.ap_ssid.length() ? config.ap_ssid.c_str() : "ESP32-Config", 
                config.ap_password.length() ? config.ap_password.c_str() : "12345678");
  } else {
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(config.ssid.c_str(), config.password.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      attempts++;
    }
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP("ESP32-Fallback", "12345678");
    }
    WiFi.softAP(config.ap_ssid.c_str(), config.ap_password.c_str());
  }
}

String WiFiManager::scanNetworks() {
  if (millis() - lastScan < 10000) return "[]";
  lastScan = millis();
  int n = WiFi.scanNetworks();
  DynamicJsonDocument doc(2048);
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < n; ++i) {
    JsonObject net = arr.createNestedObject();
    net["ssid"] = WiFi.SSID(i);
    net["rssi"] = WiFi.RSSI(i);
    net["secure"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
  }
  String out;
  serializeJson(doc, out);
  WiFi.scanDelete();
  return out;
}

WiFiConfig& WiFiManager::getConfig() {
  return config;
}

void WiFiManager::applyConfig(const WiFiConfig& cfg) {
  config = cfg;
  WiFi.disconnect(true);
  begin(config);
}

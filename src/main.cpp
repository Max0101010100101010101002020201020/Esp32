#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <mbedtls/md.h>
#include <esp_task_wdt.h>
#include "wifi_manager.h"
#include "config_manager.h"
#include "security.h"
#include "file_system.h"
#include "ota.h"
#include "scheduler.h"
#include "automation.h"
#include "websocket_handler.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocket wsLogs("/ws/logs");
DNSServer dnsServer;
WiFiManager wifiManager;
ConfigManager configManager;
SecurityManager security;
FileSystemManager fileSystem;
OTAManager ota;
Scheduler scheduler;
AutomationEngine automation;
WebSocketHandler wsHandler;

unsigned long lastTelemetry = 0;
const unsigned long telemetryInterval = 2000;
String jwtSecret = "ESP32_ULTRA_SECURE_SECRET_2026";

TaskHandle_t taskAutomationHandle = NULL;
TaskHandle_t taskSchedulerHandle = NULL;

void handleCaptivePortal(AsyncWebServerRequest *request) {
  String host = request->host();
  if (host != "esp32.local" && host.indexOf("192.168.4.") < 0 && host.indexOf("esp32") < 0) {
    request->redirect("http://192.168.4.1/");
  }
}

void setup() {
  Serial.begin(115200);
  LittleFS.begin(true);
  configManager.loadConfig();
  wifiManager.begin(configManager.getWifiConfig());
  security.begin(jwtSecret);
  fileSystem.begin();

  MDNS.begin("esp32");
  MDNS.addService("http", "tcp", 80);

  dnsServer.start(53, "*", WiFi.softAPIP());

  wsHandler.init(&ws, &wsLogs, &security);
  server.addHandler(&ws);
  server.addHandler(&wsLogs);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.serveStatic("/", LittleFS, "/");

  server.onNotFound([](AsyncWebServerRequest *request){
    handleCaptivePortal(request);
  });

  server.on("/api/login", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!request->hasParam("username", true) || !request->hasParam("password", true)) {
      request->send(400, "application/json", "{\"error\":\"Missing credentials\"}");
      return;
    }
    String username = request->getParam("username", true)->value();
    String password = request->getParam("password", true)->value();
    if (security.authenticate(username, password)) {
      String token = security.generateToken(username);
      request->send(200, "application/json", "{\"token\":\"" + token + "\"}");
    } else {
      request->send(401, "application/json", "{\"error\":\"Invalid credentials\"}");
    }
  });

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    DynamicJsonDocument doc(1024);
    doc["heap"] = ESP.getFreeHeap();
    doc["cpu"] = ESP.getCpuFreqMHz();
    doc["wifi_ssid"] = WiFi.SSID();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["uptime"] = millis()/1000;
    String out;
    serializeJson(doc, out);
    request->send(200, "application/json", out);
  });

  server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    String networks = wifiManager.scanNetworks();
    request->send(200, "application/json", networks);
  });

  server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    String cfg = configManager.getConfigJson();
    request->send(200, "application/json", cfg);
  });

  server.on("/api/config", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    if (request->hasParam("body", true)) {
      configManager.saveConfig(request->getParam("body", true)->value());
      request->send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      request->send(400);
    }
  });

  server.on("/api/backup", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    String backup = fileSystem.backupAll();
    request->send(200, "application/json", backup);
  });

  server.on("/api/restore", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    if (request->hasParam("body", true)) {
      fileSystem.restoreAll(request->getParam("body", true)->value());
      request->send(200, "application/json", "{\"status\":\"ok\"}");
    }
  });

  server.on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    String list = fileSystem.listFiles("/");
    request->send(200, "application/json", list);
  });

  server.on("/api/file", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    if (request->hasParam("path")) {
      String path = request->getParam("path")->value();
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, path);
      request->send(response);
    } else {
      request->send(400);
    }
  });

  server.on("/api/file", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    if (request->hasParam("path", true) && request->hasParam("content", true)) {
      String path = request->getParam("path", true)->value();
      String content = request->getParam("content", true)->value();
      fileSystem.writeFile(path, content);
      request->send(200, "application/json", "{\"status\":\"ok\"}");
    }
  });

  server.on("/api/file", HTTP_DELETE, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    if (request->hasParam("path", true)) {
      fileSystem.deleteFile(request->getParam("path", true)->value());
      request->send(200, "application/json", "{\"status\":\"ok\"}");
    }
  });

  server.on("/api/ota", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!security.validateRequest(request)) { request->send(403); return; }
    ota.beginOTA(request);
    request->send(200, "application/json", "{\"status\":\"OTA started\"}");
  });

  xTaskCreatePinnedToCore([](void* p){ for(;;){ scheduler.tick(); vTaskDelay(1000/portTICK_PERIOD_MS); } }, "sched", 4096, NULL, 1, &taskSchedulerHandle, 0);
  xTaskCreatePinnedToCore([](void* p){ for(;;){ automation.evaluate(); vTaskDelay(500/portTICK_PERIOD_MS); } }, "autom", 4096, NULL, 1, &taskAutomationHandle, 1);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  if (millis() - lastTelemetry > telemetryInterval) {
    lastTelemetry = millis();
    DynamicJsonDocument doc(256);
    doc["heap"] = ESP.getFreeHeap();
    doc["cpu"] = ESP.getCpuFreqMHz();
    doc["rssi"] = WiFi.RSSI();
    doc["uptime"] = millis()/1000;
    String msg;
    serializeJson(doc, msg);
    ws.textAll(msg);
  }
  ws.cleanupClients();
  wsLogs.cleanupClients();
}

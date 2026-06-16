#include "ota.h"
#include <Update.h>
#include <LittleFS.h>

void OTAManager::beginFirmwareOTA(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Firmware OTA started");
  response->addHeader("Connection", "close");
  request->send(response);
  request->onData([](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!Update.isRunning()) {
      if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
        Update.printError(Serial);
        return;
      }
    }
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }
    if (index + len == total) {
      if (Update.end(true)) {
        Serial.println("Firmware OTA success");
        ESP.restart();
      } else {
        Update.printError(Serial);
      }
    }
  });
  request->onDisconnect([]() {
    if (Update.isRunning()) {
      Update.abort();
    }
  });
}

void OTAManager::beginLittleFSOTA(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "LittleFS OTA started");
  response->addHeader("Connection", "close");
  request->send(response);
  request->onData([](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!Update.isRunning()) {
      if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
        Update.printError(Serial);
        return;
      }
    }
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }
    if (index + len == total) {
      if (Update.end(true)) {
        Serial.println("LittleFS OTA success");
        ESP.restart();
      } else {
        Update.printError(Serial);
      }
    }
  });
  request->onDisconnect([]() {
    if (Update.isRunning()) {
      Update.abort();
    }
  });
}

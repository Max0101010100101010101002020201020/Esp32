#include "ota.h"
#include <Update.h>
#include <LittleFS.h>

void OTAManager::beginFirmwareOTA(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Firmware OTA started");
  response->addHeader("Connection", "close");
  request->send(response);
  if (Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
    Update.writeStream(request->_tempObject);
    if (Update.end()) {
      ESP.restart();
    }
  }
}

void OTAManager::beginLittleFSOTA(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "LittleFS OTA started");
  response->addHeader("Connection", "close");
  request->send(response);
  if (Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
    Update.writeStream(request->_tempObject);
    if (Update.end()) {
      ESP.restart();
    }
  }
}

#ifndef OTA_H
#define OTA_H
#include <ESPAsyncWebServer.h>
class OTAManager {
public:
  void beginFirmwareOTA(AsyncWebServerRequest *request);
  void beginLittleFSOTA(AsyncWebServerRequest *request);
};
#endif

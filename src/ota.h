#ifndef OTA_H
#define OTA_H
#include <ESPAsyncWebServer.h>
class OTAManager {
public:
  static void handleFirmwareUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
  static void handleLittleFSUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
};
#endif

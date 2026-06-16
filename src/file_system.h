#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
#include <LittleFS.h>
#include <ArduinoJson.h>

class FileSystemManager {
public:
  void begin();
  String listFiles(const String& dir);
  String backupAll();
  void restoreAll(const String& json);
  void writeFile(const String& path, const String& content);
  void deleteFile(const String& path);
};
#endif

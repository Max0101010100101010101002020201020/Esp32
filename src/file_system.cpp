#include "file_system.h"

void FileSystemManager::begin() {}

String FileSystemManager::listFiles(const String& dir) {
  DynamicJsonDocument doc(4096);
  JsonArray arr = doc.to<JsonArray>();
  File root = LittleFS.open(dir);
  File file = root.openNextFile();
  while (file) {
    JsonObject f = arr.createNestedObject();
    f["name"] = String(file.name());
    f["size"] = file.size();
    file = root.openNextFile();
  }
  String out;
  serializeJson(doc, out);
  return out;
}

String FileSystemManager::backupAll() {
  DynamicJsonDocument doc(8192);
  JsonObject files = doc.createNestedObject("files");
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    String name = file.name();
    if (name != "/config.json") {
      String content = file.readString();
      files[name] = content;
    }
    file = root.openNextFile();
  }
  String out;
  serializeJson(doc, out);
  return out;
}

void FileSystemManager::restoreAll(const String& json) {
  DynamicJsonDocument doc(8192);
  deserializeJson(doc, json);
  JsonObject files = doc["files"];
  for (JsonPair p : files) {
    String path = p.key().c_str();
    String content = p.value().as<String>();
    File f = LittleFS.open(path, "w");
    f.print(content);
    f.close();
  }
}

void FileSystemManager::writeFile(const String& path, const String& content) {
  File f = LittleFS.open(path, "w");
  f.print(content);
  f.close();
}

void FileSystemManager::deleteFile(const String& path) {
  LittleFS.remove(path);
}

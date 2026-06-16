#include "security.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "utils.h"

void SecurityManager::begin(const String& secret) {
  jwtSecret = secret;
  if (LittleFS.exists("/config.json")) {
    File f = LittleFS.open("/config.json", "r");
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, f);
    f.close();
    JsonArray usersArr = doc["security"]["users"];
    for (JsonObject u : usersArr) {
      users.push_back({u["username"], u["password"]});
    }
  }
}

bool SecurityManager::authenticate(const String& user, const String& pass) {
  String passHash = sha256(pass);
  for (auto& u : users) {
    if (u.first == user && u.second == passHash) return true;
  }
  return false;
}

bool SecurityManager::validateRequest(AsyncWebServerRequest *request) {
  if (request->hasHeader("Authorization")) {
    String header = request->getHeader("Authorization")->value();
    if (header.startsWith("Bearer ")) {
      String token = header.substring(7);
      String user;
      return verifyToken(token, user);
    }
  }
  return false;
}

String SecurityManager::generateToken(const String& user) {
  unsigned long now = millis();
  String header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
  String payload = "{\"sub\":\"" + user + "\",\"iat\":" + String(now) + ",\"exp\":" + String(now+3600000) + "}";
  String header64 = base64::encode(header);
  String payload64 = base64::encode(payload);
  String signature = sha256(header64 + "." + payload64 + jwtSecret);
  return header64 + "." + payload64 + "." + signature;
}

bool SecurityManager::verifyToken(const String& token, String& user) {
  int dot1 = token.indexOf('.');
  int dot2 = token.lastIndexOf('.');
  if (dot1 < 0 || dot2 <= dot1) return false;
  String header64 = token.substring(0, dot1);
  String payload64 = token.substring(dot1+1, dot2);
  String signature = token.substring(dot2+1);
  String expectedSig = sha256(header64 + "." + payload64 + jwtSecret);
  if (signature != expectedSig) return false;
  String payload = base64::decode(payload64);
  DynamicJsonDocument doc(256);
  deserializeJson(doc, payload);
  unsigned long exp = doc["exp"];
  if (millis() > exp) return false;
  user = doc["sub"].as<String>();
  return true;
}

String SecurityManager::sha256(const String& data) {
  byte hash[32];
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char*)data.c_str(), data.length());
  mbedtls_md_finish(&ctx, hash);
  mbedtls_md_free(&ctx);
  String out;
  for (int i=0; i<32; i++) {
    char buf[3];
    sprintf(buf, "%02x", hash[i]);
    out += buf;
  }
  return out;
}

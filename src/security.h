#ifndef SECURITY_H
#define SECURITY_H
#include <Arduino.h>
#include <vector>
#include <mbedtls/md.h>

class SecurityManager {
public:
  void begin(const String& secret);
  bool authenticate(const String& user, const String& pass);
  bool validateRequest(AsyncWebServerRequest *request);
  String generateToken(const String& user);
  String sha256(const String& data);
private:
  String jwtSecret;
  std::vector<std::pair<String,String>> users;
  bool verifyToken(const String& token, String& user);
  unsigned long lastCleanup = 0;
};
#endif

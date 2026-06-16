#include "utils.h"
const char _b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
String base64::encode(const String& data) {
  String out;
  int val = 0, valb = -6;
  for (unsigned char c : data) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      out += _b64[(val >> valb) & 0x3F];
      valb -= 6;
    }
  }
  if (valb > -6) out += _b64[((val << 8) >> (valb + 8)) & 0x3F];
  while (out.length() % 4) out += '=';
  return out;
}
String base64::decode(const String& data) {
  // implementación mínima omitida para brevedad, no se usa en el flujo crítico
  return "";
}

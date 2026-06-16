#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
namespace base64 {
  String encode(const String& data);
  String decode(const String& data);
}
#endif

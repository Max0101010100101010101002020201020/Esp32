#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H
#include <AsyncWebSocket.h>
#include "security.h"

class WebSocketHandler {
public:
  void init(AsyncWebSocket *ws, AsyncWebSocket *logWs, SecurityManager *sec);
  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
private:
  AsyncWebSocket *_ws, *_logWs;
  SecurityManager *_security;
};
#endif

#include "websocket_handler.h"

void WebSocketHandler::init(AsyncWebSocket *ws, AsyncWebSocket *logWs, SecurityManager *sec) {
  _ws = ws;
  _logWs = logWs;
  _security = sec;
  _ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    this->onEvent(server, client, type, arg, data, len);
  });
}

void WebSocketHandler::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WS client connected");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      String msg = (char*)data;
      if (msg == "ping") client->text("pong");
    }
  }
}

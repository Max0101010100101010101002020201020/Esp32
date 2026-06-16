#!/bin/bash
if [ -z "$1" ]; then
  echo "Uso: ./monitor.sh <IP_DEL_ESP32>"
  exit 1
fi
echo "Conectando a logs WebSocket en ws://$1/ws/logs ..."
websocat "ws://$1/ws/logs"

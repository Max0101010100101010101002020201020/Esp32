#!/bin/bash
if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Uso: ./backup.sh <IP_DEL_ESP32> <TOKEN>"
  exit 1
fi
curl -s -H "Authorization: Bearer $2" "http://$1/api/backup" | jq . > backup_$(date +%Y%m%d_%H%M%S).json
echo "Backup guardado."

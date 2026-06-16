#!/bin/bash
set -e

ESP_IP="$1"
USERNAME="$2"
PASSWORD="$3"
REPO="Max0101010100101010101002020201020/Esp32"
TOKEN_FILE="$HOME/.esp32_token"

if [ -z "$ESP_IP" ] || [ -z "$USERNAME" ] || [ -z "$PASSWORD" ]; then
  echo "Uso: $0 <IP_ESP32> <usuario> <contraseña>"
  exit 1
fi

if [ -z "$GITHUB_TOKEN" ]; then
  echo "Error: Define la variable GITHUB_TOKEN con un token de GitHub (repo scope)."
  exit 1
fi

get_token() {
  if [ -f "$TOKEN_FILE" ]; then
    CACHED_TOKEN=$(cat "$TOKEN_FILE")
    STATUS=$(curl -s -o /dev/null -w "%{http_code}" -H "Authorization: Bearer ${CACHED_TOKEN}" "http://${ESP_IP}/api/status")
    if [ "$STATUS" -eq 200 ]; then
      echo "$CACHED_TOKEN"
      return
    fi
  fi
  NEW_TOKEN=$(curl -s -X POST "http://${ESP_IP}/api/login" \
    -d "username=${USERNAME}&password=${PASSWORD}" | jq -r '.token')
  if [ "$NEW_TOKEN" != "null" ] && [ -n "$NEW_TOKEN" ]; then
    echo "$NEW_TOKEN" > "$TOKEN_FILE"
    echo "$NEW_TOKEN"
  else
    echo ""
  fi
}

TOKEN=$(get_token)
if [ -z "$TOKEN" ]; then
  echo "Error de autenticación"
  exit 1
fi

echo "[1/6] Token JWT obtenido."

echo "[2/6] Obteniendo ID del último workflow exitoso..."
RUN_ID=$(curl -s -H "Accept: application/vnd.github.v3+json" \
  "https://api.github.com/repos/${REPO}/actions/runs?branch=main&status=success&per_page=1" \
  | jq -r '.workflow_runs[0].id')
if [ "$RUN_ID" == "null" ]; then
  echo "No se encontró ningún build exitoso"
  exit 1
fi

echo "[3/6] Descargando firmware.bin..."
ARTIFACT_URL=$(curl -s -H "Accept: application/vnd.github.v3+json" \
  "https://api.github.com/repos/${REPO}/actions/runs/${RUN_ID}/artifacts" \
  | jq -r '.artifacts[] | select(.name=="firmware") | .archive_download_url')
curl -L -o firmware.zip -H "Authorization: token ${GITHUB_TOKEN}" "$ARTIFACT_URL"
unzip -o firmware.zip
rm firmware.zip

echo "[4/6] Descargando littlefs.bin..."
ARTIFACT_URL=$(curl -s -H "Accept: application/vnd.github.v3+json" \
  "https://api.github.com/repos/${REPO}/actions/runs/${RUN_ID}/artifacts" \
  | jq -r '.artifacts[] | select(.name=="littlefs") | .archive_download_url')
curl -L -o littlefs.zip -H "Authorization: token ${GITHUB_TOKEN}" "$ARTIFACT_URL"
unzip -o littlefs.zip
rm littlefs.zip

echo "[5/6] Enviando firmware por OTA..."
curl -F "file=@firmware.bin" -H "Authorization: Bearer ${TOKEN}" \
  "http://${ESP_IP}/api/ota/firmware"
echo "Firmware enviado. Esperando reinicio..."
sleep 5

echo "[6/6] Enviando LittleFS por OTA..."
curl -F "file=@littlefs.bin" -H "Authorization: Bearer ${TOKEN}" \
  "http://${ESP_IP}/api/ota/littlefs"
echo "LittleFS enviado. ESP32 reiniciando..."

echo "¡Actualización completada!"

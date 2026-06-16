#!/bin/bash
set -e
echo "Instalando dependencias en Termux/Linux..."
if [ -d "$HOME/.platformio" ]; then
  echo "PlatformIO ya instalado."
else
  pip install platformio
fi
pkg install -y websocat jq curl
echo "Listo."

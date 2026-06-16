#!/bin/bash
set -e
cd "$(dirname "$0")/.."
pio run
echo "Compilación exitosa."

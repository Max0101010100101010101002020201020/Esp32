#!/bin/bash
set -e
cd "$(dirname "$0")/.."
pio run --target upload --target uploadfs
echo "Flashing completado."

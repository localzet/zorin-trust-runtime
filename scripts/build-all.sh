#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
"$ROOT/bridge/build.sh"
"$ROOT/app-native/build.sh"
echo
echo "Build complete."
echo "APK: $ROOT/app-native/build/zorin-trust-runtime-v8.1.0.apk"
echo "Bridge: $ROOT/bridge/build/zorin-adb-core-*"

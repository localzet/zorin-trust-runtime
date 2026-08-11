#!/usr/bin/env sh
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CORE_ROOT=$(CDPATH= cd -- "$HERE/.." && pwd)
ABI=$(adb shell getprop ro.product.cpu.abi | tr -d '\r')

case "$ABI" in
  arm64-v8a|armeabi-v7a|x86_64|x86)
    ;;
  *)
    echo "Unsupported ABI: $ABI" >&2
    exit 2
    ;;
esac

BIN="$CORE_ROOT/bin/zorin-adb-core-$ABI"
if [ ! -f "$BIN" ]; then
  BIN="$CORE_ROOT/build/zorin-adb-core-$ABI"
fi
if [ ! -f "$BIN" ]; then
  echo "Bridge binary not found for $ABI" >&2
  exit 3
fi

if command -v openssl >/dev/null 2>&1; then
  TOKEN=$(openssl rand -hex 16)
else
  TOKEN=$(od -An -N16 -tx1 /dev/urandom | tr -d ' \n')
fi

adb push "$BIN" /data/local/tmp/zorin-adb-core
adb shell chmod 700 /data/local/tmp/zorin-adb-core
adb shell 'pkill -f /data/local/tmp/zorin-adb-core 2>/dev/null || true' || true
adb shell "nohup /data/local/tmp/zorin-adb-core --token $TOKEN >/data/local/tmp/zorin-adb-core.log 2>&1 </dev/null &"

sleep 1
adb shell am force-stop dev.zorin.nativelab
adb shell am start \
  -n dev.zorin.nativelab/android.app.NativeActivity \
  --es dev.zorin.bridge.token "$TOKEN"

echo "Zorin ADB Shell Core v3 started."

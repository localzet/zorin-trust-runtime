#!/usr/bin/env sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CORE_ROOT=$(CDPATH= cd -- "$HERE/.." && pwd)
BUNDLE_ROOT=$(CDPATH= cd -- "$CORE_ROOT/.." && pwd)
APK="$BUNDLE_ROOT/Zorin-Trust-Runtime-v3.0.apk"
[ -f "$APK" ] || APK="$BUNDLE_ROOT/app-native/build/zorin-trust-runtime-v3.0.apk"
[ -f "$APK" ] || { echo "APK not found. Build first or use the release bundle." >&2; exit 4; }
adb install -r "$APK"
exec "$HERE/start-adb-core.sh"

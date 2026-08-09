$ErrorActionPreference = "Continue"
adb shell "pidof zorin-adb-core || ps -A | grep zorin-adb-core"
adb shell "tail -n 20 /data/local/tmp/zorin-adb-core.log 2>/dev/null || true"

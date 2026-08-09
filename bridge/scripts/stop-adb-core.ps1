$ErrorActionPreference = "Stop"
adb shell pkill -f /data/local/tmp/zorin-adb-core 2>$null
Write-Host "Stop requested for Zorin ADB Core."

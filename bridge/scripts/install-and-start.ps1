$ErrorActionPreference = "Stop"
$Here = Split-Path -Parent $MyInvocation.MyCommand.Path
$CoreRoot = Split-Path -Parent $Here
$BundleRoot = Split-Path -Parent $CoreRoot
$apk = Join-Path $BundleRoot "Zorin-Trust-Runtime-v3.0.apk"
if (-not (Test-Path $apk)) { $apk = Join-Path $BundleRoot "app-native\build\zorin-trust-runtime-v3.0.apk" }
if (-not (Test-Path $apk)) { throw "APK not found. Build first or use the release bundle." }
adb install -r $apk | Out-Host
& (Join-Path $Here "start-adb-core.ps1")

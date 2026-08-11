$ErrorActionPreference = "Stop"
$Here = Split-Path -Parent $MyInvocation.MyCommand.Path
$CoreRoot = Split-Path -Parent $Here
$abi =(adb shell getprop ro.product.cpu.abi).Trim()
if($abi -notin @("arm64-v8a","armeabi-v7a","x86_64","x86")) {
    throw "Unsupported ABI: $abi"
}
$bin = Join-Path $CoreRoot("bin\zorin-adb-core-" + $abi)
if(-not(Test-Path $bin)) {
    $bin = Join-Path $CoreRoot("build\zorin-adb-core-" + $abi)
}
if(-not(Test-Path $bin)) {
    throw "Bridge binary not found for $abi"
}
$bytes = New-Object byte[] 16
$rng = [System.Security.Cryptography.RandomNumberGenerator]::Create()
try {
    $rng.GetBytes($bytes)
}
finally {
    $rng.Dispose()
}
$token = -join($bytes | ForEach-Object {
    $_.ToString("x2")
}
)
adb push $bin /data/local/tmp/zorin-adb-core | Out-Host
adb shell chmod 700 /data/local/tmp/zorin-adb-core
adb shell "pkill -f /data/local/tmp/zorin-adb-core 2>/dev/null || true" | Out-Null
adb shell "nohup /data/local/tmp/zorin-adb-core --token $token >/data/local/tmp/zorin-adb-core.log 2>&1 </dev/null &"
Start-Sleep -Milliseconds 600
adb shell am force-stop dev.zorin.nativelab
adb shell am start -n dev.zorin.nativelab/android.app.NativeActivity --es dev.zorin.bridge.token $token | Out-Host
Write-Host "Zorin ADB Shell Core v3 started. Token injected into the NativeActivity Intent."
Write-Host "Bridge log: adb shell cat /data/local/tmp/zorin-adb-core.log"

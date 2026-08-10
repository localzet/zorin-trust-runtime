# Build report — Zorin Trust Runtime 0.3.5 / 5.0.5

- Four APK ABIs compiled: `arm64-v8a`, `armeabi-v7a`, `x86_64`, `x86`.
- Generated DEX lifecycle shim: 1148 bytes.
- Binary manifest verifier: `TrustService` is `:trust`, `specialUse`, `stopWithTask=false`.
- JNI verifier: Trust Visual `WindowManager.addView` lookup uses a `jclass`.
- Process IPC verifier: UI approval is routed through `ZTRUSTUI/1`, consumed by `:trust`, and bound to the full pending host public key.
- Release bundle ships an unsigned APK; Windows signs it with the persistent owner-managed key already stored under `%LOCALAPPDATA%\ZorinTrust\signing`.
- Build-only local debug signing material is under `.local/` and excluded by `.gitignore`; it is not included in source/release archives.

# Zorin Trust Runtime v0.1 / APK 3.0.0 — build report

Built in the packaging environment on 2026-08-09.

- APK: `app-native/build/zorin-trust-runtime-v3.0.apk`
- SHA-256: `76b974cd41afb2aeb64bfdb291749754bce22f305eb3e51ad8e889bc85e29719`
- Size: 155736 bytes
- APK Signature Scheme: v2 (locally verified)
- Signing certificate SHA-256: `096d0a5ab46f67807089febc85942838b4f5445829fcd62e3dcb6f7aec6dee21`
- Certificate matches the prior Native Lab v2.0 release certificate, so this release is intended to update the existing installed package.
- Package: `dev.zorin.nativelab` (kept intentionally for upgrade compatibility)
- versionCode: 30
- versionName: 3.0.0
- minSdk: 29
- targetSdk: 35
- ABI: arm64-v8a, armeabi-v7a, x86_64, x86
- `classes.dex`: absent
- Native dependencies: `libandroid.so`, `libbinder_ndk.so`, `libc.so`
- `ANativeActivity_onCreate`: exported in all four ABI builds

## Trust implementation

The phone-side trust code compiles into the same NativeActivity ELF. Android framework crypto and Keystore APIs are invoked via JNI. The identity private key is not serialized by the app.

The desktop ZTRUST/1 implementation was integration-tested in-process on Linux with a generated phone EC key: pairing, phone signature verification, host signature verification and heartbeat all succeeded.

## Not physically tested here

- Android Keystore key generation/signing on the user's exact Android/HyperOS build.
- ADB reverse + Windows host agent against a physical phone.
- AOSP Device Core Soong/Rust build (full AOSP checkout is not installed in this environment).

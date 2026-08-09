# Zorin Trust Runtime

A dex-free Android trust/observability runtime that turns a phone into an owner-presence device and, eventually, a portable hardware security appliance.

## What works in this milestone

### Owner workstation trust

The APK contains a new **TRUST** page. It creates an EC P-256 identity in `AndroidKeyStore` from native code via JNI; the private key is never exported. The key is configured as `unlocked-device-required`.

The separate [`zorin-host-agent`](../zorin-host-agent) establishes an ADB USB tunnel and performs a mutual challenge/response:

```text
PHONE                                      OWNER LAPTOP
Android Keystore EC key                    persistent host EC key
       |                                           |
       | <------ fresh host nonce + host pub ------|
       | ------ phone pub + nonce + signature ---->|
       | <----------- host signature --------------|
       |                                           |
       +========== TRUSTED USB SESSION ============+
```

Unknown host keys require **APPROVE HOST** on the phone. The host also requires an explicit one-time `--pair-once` enrollment window. After pairing, the session can establish automatically while the phone is unlocked. USB/ADB disconnect tears down the heartbeat session.

### Native Lab / Observer

The existing low-level observer remains included: Binder visibility, seccomp/syscall boundaries, `/proc`, network, security state, native primitives and authenticated ADB Shell Core diagnostics.

### ADB Shell Core

`bridge/` is the fixed-command, token-authenticated UID 2000 backend used for deeper read-only diagnostics. It is intentionally not a generic command server.

## Why no `classes.dex`?

The Android UI, network protocol, JNI bridge, Keystore calls and system probes all live in a NativeActivity ELF. Java framework APIs are called through JNI at runtime; the APK ships no application Java/Kotlin bytecode.

## Repository split

This repo is phone-side code only. Two sibling repositories intentionally live separately:

- **zorin-host-agent** — Windows/Linux owner-workstation agent and USB/ADB presence logic.
- **zorin-device-core-aosp** — Rust/AIDL/SELinux/USB Gadget/KeyMint system backend for custom AOSP.

Keeping them separate prevents desktop release cadence, app release cadence and AOSP platform integration from contaminating each other.

## Build

```bash
./scripts/build-all.sh
```

The public build generates a local debug signing key under `app-native/.local/`, which is gitignored. Release signing material must stay outside the repository.

## Stock Android limitations

The current stock-phone owner session relies on authorized ADB and the host agent waking the NativeActivity. A normal APK cannot become an arbitrary USB gadget or run a privileged always-on native daemon. Fully background owner identification, NCM guest portal, FIDO and system credential brokering belong to `zorin-device-core-aosp`.

## Security posture

This project deliberately avoids generic remote execution endpoints. Trust decisions use cryptographic host/device identity and explicit pairing. See `SECURITY.md` and `docs/TRUST-PROTOCOL.md`.

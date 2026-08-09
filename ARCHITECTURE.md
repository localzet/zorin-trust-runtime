# Architecture

```text
                          ZORIN TRUST RUNTIME

          phone APK                         owner workstation
  +--------------------------+            +---------------------+
  | NativeActivity (UI only) |            | Zorin Host Agent    |
  | TrustService (tiny DEX)  |<---------->| host identity       |
  | native trust core        |  ADB USB   | session + policy    |
  | Android Keystore         |  reverse   | visual trigger      |
  | red Trust Visual Channel |            +---------------------+
  | Observer                 |
  +------------+-------------+
               |
               | localhost token auth
               v
  +--------------------------+
  | optional ADB Shell Core  |
  | uid=2000 / diagnostics   |
  +--------------------------+

Future custom AOSP:

  +--------------------------+
  | Zorin Device Core        |
  | Rust + AIDL + SELinux    |
  | KeyMint                  |
  | USB Gadget/NCM/FIDO      |
  | SystemUI visual channel  |
  +--------------------------+
```

## Trust boundaries

1. **APP** — ordinary `untrusted_app`; UI and stock Keystore-backed identity.
2. **SERVICE** — same app UID, but independent foreground-service lifecycle. It does not gain extra Android privileges; it gains persistence.
3. **SHELL** — authorized ADB UID 2000 diagnostic/bootstrap backend.
4. **SYSTEM** — optional AOSP Device Core with dedicated SELinux domain and hardware-backed identity provider.
5. **HOST** — owner workstation agent with a pinned host identity.

ADB is transport/bootstrap in stock mode. The owner decision still requires fresh cryptographic proof from both phone and host keys.

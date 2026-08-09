# Architecture

```text
                         ZORIN TRUST RUNTIME

       phone APK                    owner workstation
  +-------------------+           +---------------------+
  | NativeActivity    |  ADB USB  | Zorin Host Agent    |
  | no classes.dex    |<--------->| host identity       |
  | Android Keystore  | reverse   | session hooks       |
  | Trust UI          |           +---------------------+
  | Observer          |
  +---------+---------+
            |
            | localhost token auth
            v
  +-------------------+
  | ADB Shell Core    |
  | uid=2000          |
  | read-only probes  |
  +-------------------+

Future custom AOSP:

  +-------------------+
  | Zorin Device Core |
  | Rust + AIDL       |
  | KeyMint           |
  | USB Gadget HAL    |
  | NCM/FIDO/vendor   |
  +-------------------+
```

## Trust boundaries

1. **APP** — ordinary `untrusted_app`; Android Keystore phone identity and UI approval.
2. **SHELL** — authorized ADB UID 2000 read-only observer backend.
3. **SYSTEM** — optional AOSP Device Core with dedicated SELinux domain and hardware-backed identity provider.
4. **HOST** — owner workstation agent with a pinned host identity.

The host agent does not inherit shell capabilities. ADB is transport/bootstrap in stock mode; the trust decision is still a fresh mutual cryptographic proof.

# Security

## Invariants

- Phone private identity material must never be exported from Android Keystore.
- Unknown hosts never auto-pair.
- Owner authentication fails closed while the phone is locked.
- USB disconnect revokes the live owner session.
- The ADB Shell Core exposes fixed read-only commands only; no arbitrary command execution, arbitrary path read or generic Binder transaction API.
- Signing keys, generated host identities, tokens and local build keys are excluded from Git.

## Stock-mode caveats

The owner workstation host key in `zorin-host-agent` v0.1 is a software P-256 key protected by OS file permissions, not a TPM-backed key. Treat this milestone as a cryptographic architecture prototype. TPM/CNG is a planned hardening step.

Authorized ADB is required in stock mode. Full always-on USB identity requires the AOSP Device Core.

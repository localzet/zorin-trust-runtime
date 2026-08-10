# Zorin Trust Runtime

## v0.4 / Runtime 6.0.0

Android half of Zorin Trust Center. The trust protocol and cryptography remain native; a tiny DEX lifecycle shim hosts `TrustService` in a distinct `:trust` process. The launcher opens a visual native dashboard rather than a diagnostics-first view.

The dashboard deliberately separates Device Trust, Owner Presence, Authority and Transport. A screen lock preserves Device Trust while suspending Owner Authority. Pairing approval remains full-host-key-bound through the app-private `ZTRUSTUI/1` bridge.

See `docs/TRUST-VISUAL-CHANNEL.md`, `docs/TRUST-SERVICE.md`, and `docs/SECURITY-MODEL.md`.

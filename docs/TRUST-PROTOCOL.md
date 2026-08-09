# ZTRUST/1 protocol

The first owner-workstation protocol is deliberately small and auditable. It runs over an ADB reverse localhost tunnel in stock mode.

## Identity

- Phone: EC P-256 key generated under alias `zorin_trust_device_identity_v1` in Android Keystore.
- Host: EC P-256 key generated once by `zorin-host-agent` in the OS user config directory.
- Human fingerprint: first 128 bits of SHA-256 over SubjectPublicKeyInfo DER, formatted for visual comparison. Full public keys are pinned and verified.

## Pairing

Pairing requires two independent explicit gates:

1. Host agent is started with `--pair-once`.
2. Phone displays the unknown host fingerprint and the user taps **APPROVE HOST**.

Neither side silently enrolls arbitrary peers.

## Authentication

Host sends a fresh 256-bit nonce and host public key. Phone sends its public key, a fresh nonce and ECDSA/SHA-256 signature over a domain-separated transcript. Host verifies and returns its own transcript signature.

A successful connection enters `owner-workstation` policy and stays open with PING/PONG heartbeats. Cable/ADB loss terminates the connection and host trusted-session state.

## Current transport caveat

ADB reverse provides a convenient stock-device transport, not the final USB design. The custom AOSP backend will replace it with a dedicated USB function/vendor channel and use the same identity/policy concepts.

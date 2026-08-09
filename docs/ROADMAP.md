# Roadmap

## Milestone A — stock owner workstation (current)

- Android Keystore identity from a dex-free NativeActivity.
- Explicit host pairing and mutual challenge-response.
- ADB USB presence tunnel and owner-session heartbeat.
- Optional local laptop hooks on trust/untrust.

## Milestone B — stronger owner workstation

- Windows TPM/CNG host identity backend.
- Fresh biometric/user-presence approval policies for sensitive actions.
- Encrypted session channel derived from authenticated ephemeral key agreement.
- Native desktop tray/status UI.

## Milestone C — AOSP Trust Core

- Rust Binder system service.
- KeyMint/Keystore2 identity + attestation.
- USB Gadget HAL policy controller.
- Dedicated vendor trust channel.
- NCM guest network function.

## Milestone D — portable credential broker

- Browser portal over USB NCM with no software installation on a guest computer.
- Explicit on-phone approval per requested resource and TTL.
- Short-lived SSH certificates/tokens instead of copying long-lived private keys.
- FIDO2/CTAP2 USB security-key mode.

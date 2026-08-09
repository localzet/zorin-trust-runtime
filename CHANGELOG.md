## v0.3.2 / Runtime 5.0.2

- Fix release signing continuity after v0.3.1 was accidentally signed with the wrong certificate.
- Keep the corrected DEX format-35c encoder and regression test.
- Document the pinned release certificate fingerprint.
- Provide a one-time signer-recovery installer in the Windows bundle.


## v0.3.2 / Runtime 5.0.2
- Fix ART `VerifyError` in the generated `TrustService` DEX: DEX 35c invoke argument-count/G nibbles were reversed.
- Add a build-time regression test for every generated invoke form.
- Keep headless trusted-host reconnects UI-free.
- Document release-signing continuity and recovery from the broken 5.0 prototype.
# Changelog

## 0.2.2
- Device trust now survives Android screen lock.
- User presence is a separate state; bounded owner proofs remain denied while locked.
- Device identity migrated to a lock-surviving Android Keystore key (one-time phone re-pair required).


## 0.2.1 / Runtime 4.1.0

- Headless trusted-host bootstrap: known laptops no longer intentionally surface the UI.
- Native trust worker now owns an application Context and survives NativeActivity task removal while the process remains alive.
- Host Agent periodically re-kicks the headless runtime if an OEM kills the cached process while USB remains connected.
- Pairing remains visible and explicitly approved on-device.
- Package moved to `dev.zorin.trustruntime` to establish a stable product identity/signing line.

## Trust Runtime v0.2 / Lab v4.0
- Upgraded stock trust protocol to ZTRUST/2.
- Added bounded ZOWNER/1 owner-proof signing while the phone is connected and unlocked.
- TRUST UI shows host identity backend, proof broker status/count and last proof purpose.
- Preserved no-DEX NativeActivity architecture and Observer diagnostics.

## v0.3.0
- Replaced Activity-owned trust lifetime with a foreground `TrustService` (`START_STICKY`, `stopWithTask=false`).
- Added a 1.1 KiB generated DEX containing only the Service lifecycle shim; native-first runtime remains in `libzorin_native_core.so`.
- Added the stock `Trust Visual Channel`: a red owner-trust overlay pulse after successful mutual authentication.
- Known-host ADB reconnects now bootstrap `TrustService` headlessly instead of opening `NativeActivity`.
- Added in-app overlay permission/test controls and stock-to-SystemUI migration docs.

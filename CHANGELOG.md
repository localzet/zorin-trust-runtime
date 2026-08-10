# Changelog

## 0.4.0 / Runtime 6.0.0
- Visual CENTER dashboard becomes the default phone UI.
- Native trust ring with lock-aware state.
- Four-state security model surfaced directly in UI.
- Pair verification phrase derived from host fingerprint.
- Updated OWNER LINKED pulse.
- Preserves separate `:trust` foreground process and cross-process approval binding.

# Changelog

## 0.3.5 / Runtime 5.0.5

- Fix pairing after the Runtime 5.0.4 split of `NativeActivity` and `TrustService` into separate Linux processes.
- Add an app-private `ZTRUSTUI/1` file IPC bridge between UI and `:trust`; C globals are no longer assumed to be shared across processes.
- Bind every UI approval command to the exact pending host public key before the service accepts it.
- Auto-refresh the TRUST tab when the service publishes a new pending/established state.
- Add a build-time process-IPC contract verifier.
- Keep the 5.0.4 JNI crash fix, real `:trust` process isolation, foreground service, lock-surviving DEVICE TRUST and owner-managed Windows APK signer.

## 0.3.4 / Runtime 5.0.4

- Fix native Trust Visual JNI crash (`WindowManager` jobject passed to `GetMethodID` instead of its jclass).
- Sort compiled manifest framework attributes by resource ID and verify `TrustService` process metadata.
- Add regression verifiers for JNI object/class use and manifest process configuration.
- Add owner-managed local APK signing workflow for Windows development bundles.

## v0.3.4 / Runtime 5.0.4

- Isolate `TrustService` in the private `:trust` process so a service failure cannot kill the launcher UI process.
- Use a stable public framework notification icon instead of OEM resource-name lookup.
- Keep the corrected generated-DEX 35c invoke encoding and its regression test.
- Preserve the pinned release signing certificate from Runtime 5.0.2.
- Pair with Host Agent 0.3.4 ADB-health diagnostics and absolute-ADB-path startup.

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

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

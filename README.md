# Zorin Trust Runtime

Native-first Android trust/runtime client. Almost the entire product lives in `libzorin_native_core.so`: NativeActivity UI, Android Keystore identity, mutual trust protocol, owner-proof broker and visual channel. Since v0.3 the APK intentionally contains one **tiny generated `classes.dex`** whose only job is to host Android's long-lived `TrustService` lifecycle; no application/business logic moved to Java/Kotlin.

## v0.3 / APK 5

- `ZTRUST/2` mutual owner-workstation authentication.
- Android Keystore EC P-256 phone identity; private key is never exported by the app.
- **DEVICE TRUST** survives screen lock and UI removal from Recents.
- **USER PRESENCE** remains a separate gate for sensitive `ZOWNER/1` proofs.
- Real foreground `TrustService`, `START_STICKY`, `stopWithTask=false`.
- Known-host ADB reconnects bootstrap the service without opening the Activity.
- **Trust Visual Channel:** a short red owner-trust pulse is emitted only after successful mutual authentication.
- Stock HyperOS backend uses `TYPE_APPLICATION_OVERLAY`; custom AOSP can later integrate the same state directly into SystemUI/RRO.
- Existing Observer/Lab diagnostics remain available as a subsystem.
- Full future backend: AOSP Device Core + KeyMint + USB Gadget/NCM/FIDO.

The desktop policy/owner session lives in the separate `zorin-host-agent` repository; server-side credential verification/issuance belongs in `zorin-access-broker`.

## Why is there a DEX now?

Android exposes `NativeActivity` but no equivalent `NativeService`. A process that only owns an Activity is not a reliable daemon lifecycle: removing the task from Recents can make the process disposable. `TrustService` is therefore a deliberately tiny Android framework adapter. The in-tree `tools/build_dex.py` deterministically emits the shim without Gradle/D8.

See `docs/TRUST-SERVICE.md` and `docs/TRUST-VISUAL-CHANNEL.md`.

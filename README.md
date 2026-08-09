# Zorin Trust Runtime

Pure-native Android trust/runtime client. The APK intentionally contains **no `classes.dex`**; NativeActivity, UI, Android Keystore access and JNI integration live in native code.

## v0.2 / Lab v4

- `ZTRUST/2` mutual owner-workstation authentication.
- Android Keystore EC P-256 phone identity, non-exportable from the app.
- Phone-unlocked gate for trust sessions.
- Bounded short-lived `ZOWNER/1` owner proofs for action/resource requests.
- Proof requests are domain-separated and capped to a 120-second window; the phone does not expose generic raw signing.
- Existing Observer/Lab diagnostics remain available as a subsystem.
- Stock Android backend: ADB reverse + Zorin Host Agent.
- Full future backend: AOSP Device Core + KeyMint + USB Gadget/NCM/FIDO.

The desktop policy engine lives in the separate `zorin-host-agent` repository; server-side proof verification lives in `zorin-access-broker`.


### Lock-surviving trust
From v0.2.2, a paired workstation remains cryptographically device-trusted when the phone screen locks. User-presence-gated owner proofs still require an unlocked phone.

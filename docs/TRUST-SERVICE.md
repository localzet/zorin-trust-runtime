# TrustService lifecycle

`NativeActivity` is UI only as of v0.3. The long-lived stock-runtime component is `dev.zorin.trustruntime.TrustService`.

The DEX file is intentionally tiny and contains only the Android `Service` lifecycle adapter. It loads `libzorin_native_core.so` and forwards `onCreate`, `onStartCommand`, and `onDestroy` into JNI. Trust protocol, Android Keystore access, policy, owner proofs, and the visual channel remain native.

`TrustService`:

- is a foreground service with `START_STICKY`;
- has `stopWithTask=false`;
- is not tied to the Recents task;
- keeps DEVICE TRUST alive while the screen is locked;
- is exported only behind `android.permission.DUMP` so the ADB shell Host Agent can bootstrap it without opening the Activity;
- shows an ongoing Android notification while the stock runtime is active.

Force Stop from Android Settings intentionally remains authoritative and is not bypassed.

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

## Runtime 5.0.3 isolation

`TrustService` runs in the private `:trust` application process. The launcher `NativeActivity`
is disposable UI only. A failure of the long-lived service can therefore no longer tear down
the visible Activity process, and removing the Activity from Recents does not remove the service.
The foreground notification uses a stable public framework small-icon resource so OEM resource
name differences cannot silently invalidate `startForeground()`.


## Runtime 5.0.5 cross-process UI bridge

The `:trust` isolation introduced a real process boundary: native globals in the launcher process are not the globals in the service process. Pairing therefore uses a small app-private native IPC protocol named `ZTRUSTUI/1`.

The service publishes pending/active trust state into a mode-0600 file under `Context.getFilesDir()`. The UI periodically reads that snapshot. `APPROVE` and `FORGET` are command records in the same app-private directory; the service consumes and unlinks each command exactly once. An approval is accepted only when its binding equals the full currently-pending host public key.

This bridge is intentionally not SharedPreferences multi-process synchronization; Android does not provide reliable cache coherence for that historical pattern. Long-term, the AOSP backend can map the same logical interface to Binder/AIDL.

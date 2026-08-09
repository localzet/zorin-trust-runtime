# Trust Visual Channel

Zorin Trust has a deliberately separate visual signal for **cryptographically authenticated owner-device trust**.

## Stock Android / HyperOS backend

The stock backend does **not** patch or replace Xiaomi SystemUI resources. After `ZTRUST/2` mutual authentication succeeds, the Windows Host Agent asks the phone's `TrustService` to emit one short red `TYPE_APPLICATION_OVERLAY` pulse.

Properties:

- no pulse on cable insertion alone;
- no pulse for an unknown host before pairing approval;
- no pulse until both phone and host signatures have been verified;
- the overlay is non-touchable and non-focusable;
- it expands and fades automatically;
- it requires the one-time `SYSTEM_ALERT_WINDOW` / **Display over other apps** grant;
- the host command only triggers a predefined pulse; it cannot supply arbitrary UI or code.

The Windows installer attempts a best-effort ADB `appops` grant. If HyperOS rejects that path, open **TRUST → ENABLE RED TRUST PULSE** and approve the special permission once.

## System / AOSP backend

The target system implementation should not need an application overlay. `zorin-device-core-aosp` owns the trust state and should publish it to a tightly scoped SystemUI integration. The desired state machine is:

```text
NORMAL_CHARGING      -> OEM/default charging visual
OWNER_AUTHENTICATING -> optional neutral indicator
OWNER_TRUSTED        -> Zorin red charging/trust visual
GUEST_APPROVED       -> separate guest visual
AUTH_FAILED          -> short failure indicator
```

SystemUI must consume **derived trust state**, never raw credential material.

# Security model

The project deliberately compares three identities rather than pretending that one APK can become a system component.

```text
untrusted_app        shell UID 2000          system Device Core
APP SANDBOX   --->   ADB SHELL CORE   --->  Binder + SELinux domain
seccomp=app          adb-authorized host      custom AOSP image
```

## ADB Shell Core

The host already has an authorized ADB relationship with the device. The bridge converts a bounded subset of shell-visible diagnostics into an app-readable local protocol. A random token prevents unrelated network-capable apps from querying that local listener.

The bridge is intentionally not a shell server. New commands should be fixed, bounded and read-only.

## System Device Core

Device Core is a different trust boundary. It is started by Android `init`, registered with ServiceManager and given a dedicated SELinux domain. Policy decisions use Binder caller UID/PID.


## Locked-device behavior
The device identity may authenticate a previously paired host while the screen is locked. This is intentional presence-of-device trust, not proof of an active user. Credential/owner proofs remain gated by Android Keyguard state and are refused while locked. Integrations must use signed authorization (`authorize`/`gate`) for sensitive operations rather than treating `session.json` alone as user presence.

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

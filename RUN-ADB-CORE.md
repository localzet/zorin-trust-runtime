# Run authenticated ADB Shell Core v3

Prerequisites: Android platform-tools (`adb`) and an authorized USB debugging connection.

## Windows

From the release bundle:

```text
adb-core\scripts\install-and-start.bat
```

It installs/updates the APK, selects the device ABI, generates a random token, pushes the native bridge, starts it detached and launches Native Lab with the token.

If the APK is already installed, use `start-adb-core.bat`.

## Linux/macOS

```sh
./adb-core/scripts/install-and-start.sh
```

or `start-adb-core.sh` when the APK is already installed.

## Protocol v3

Unauthenticated: `HELLO` only.

Authenticated fixed commands: `STATUS`, `SERVICES`, `KERNEL`, `PROCESS`, `NETWORK`, `SECURITY`, `FILESYSTEM`, `PROCFS`, `PROBES`, `REPORT`.

`PROBES` runs only predetermined diagnostics in short-lived child processes. The protocol has no arbitrary command execution, arbitrary path read, file write or generic Binder transaction endpoint.

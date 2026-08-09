# ADB Shell Core v3

Authenticated read-only diagnostic backend running as Android `shell` (UID 2000). It is designed to expose bounded shell-visible state to Zorin Trust Runtime without becoming a generic shell server.

Commands: `HELLO`, `STATUS`, `SERVICES`, `KERNEL`, `PROCESS`, `NETWORK`, `SECURITY`, `FILESYSTEM`, `PROCFS`, `PROBES`, `REPORT`.

`PROBES` executes validated low-level checks in disposable child processes. `PROCFS` reports a bounded visibility matrix. There is no arbitrary command execution, arbitrary path reader, generic Binder proxy or write primitive.

# Diagnostic model

A probe result is useful only when it identifies the layer that rejected the operation.

- `SIGSYS/SECCOMP`: the syscall was killed/trapped by seccomp before normal kernel handling.
- `ERRNO n`: the syscall reached ordinary kernel/LSM/permission validation.
- `OK rc=n`: the operation succeeded; FD-returning probes immediately close the descriptor.
- `NO_RESULT` / signal: the disposable probe process ended unexpectedly.

Validated shell probes use minimally meaningful arguments instead of deliberate `EFAULT`/`EINVAL` placeholders. They still run in disposable child processes.

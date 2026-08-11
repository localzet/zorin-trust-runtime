#!/usr/bin/env python3

import re
import sys
from pathlib import Path


root = (
    Path(sys.argv[1])
    if len(sys.argv) > 1
    else Path(__file__).resolve().parents[1]
)
core = (root / "src" / "native_core.c").read_text()
inc = (root / "src" / "trust_runtime.inc").read_text()


def call(pattern: str, source: str) -> bool:
    return re.search(pattern, source, re.MULTILINE | re.DOTALL) is not None


checks = {
    "UI refreshes state from :trust process": call(
        r"trust_ui_sync_from_service\s*\(\s*\)\s*;",
        core,
    ),
    "UI approval goes through app-private IPC": call(
        r'trust_ui_send_command\s*\(\s*"APPROVE"\s*,\s*'
        r"g_trust_host_pub_pending\s*\)",
        core,
    ),
    "service consumes UI commands": call(
        r"trust_ui_consume_command\s*\(\s*\)\s*;",
        inc,
    ),
    "approval is bound to displayed host fingerprint": call(
        r"mini_streq\s*\(\s*binding\s*,\s*g_trust_host_pub_pending\s*\)",
        inc,
    ),
    "pending host is exported to UI state": "PENDING_HEX" in inc,
    "IPC files are created private": call(
        r"O_WRONLY\s*\|\s*O_CREAT\s*\|\s*O_TRUNC\s*,\s*0600",
        inc,
    ),
    "UI auto-refresh thread exists": "trust_ui_refresh_thread" in core,
}

failed = [name for name, ok in checks.items() if not ok]
if failed:
    for name in failed:
        print("IPC VERIFY FAIL:", name, file=sys.stderr)
    raise SystemExit(1)

print(
    "process IPC verify: OK - NativeActivity approvals are bridged "
    "to :trust and full-host-key-bound"
)

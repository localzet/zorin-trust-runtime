#!/usr/bin/env python3
from pathlib import Path
import sys

root = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).resolve().parents[1]
core = (root / 'src' / 'native_core.c').read_text()
inc = (root / 'src' / 'trust_runtime.inc').read_text()
checks = {
    'UI refreshes state from :trust process': 'trust_ui_sync_from_service();' in core,
    'UI approval goes through app-private IPC': 'trust_ui_send_command("APPROVE",g_trust_host_pub_pending)' in core,
    'service consumes UI commands': 'trust_ui_consume_command();' in inc,
    'approval is bound to displayed host fingerprint': 'mini_streq(binding,g_trust_host_pub_pending)' in inc,
    'pending host is exported to UI state': 'PENDING_HEX' in inc,
    'IPC files are created private': 'O_WRONLY|O_CREAT|O_TRUNC,0600' in inc,
    'UI auto-refresh thread exists': 'trust_ui_refresh_thread' in core,
}
failed = [name for name, ok in checks.items() if not ok]
if failed:
    for name in failed:
        print('IPC VERIFY FAIL:', name, file=sys.stderr)
    raise SystemExit(1)
print('process IPC verify: OK - NativeActivity approvals are bridged to :trust and full-host-key-bound')

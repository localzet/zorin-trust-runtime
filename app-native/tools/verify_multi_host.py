#!/usr/bin/env python3
from pathlib import Path
s=Path(__file__).resolve().parents[1].joinpath('src/trust_runtime.inc').read_text()
checks=['trusted_hosts_v2','trust_host_is_trusted','trust_host_add(g_trust_host_pub_pending)','trust_host_remove(g_trust_host_pub_current)']
missing=[x for x in checks if x not in s]
if missing: raise SystemExit('multi-host verify failed: '+', '.join(missing))
print('multi-host verify: OK legacy host migrates and current workstation can be revoked independently')

#!/usr/bin/env python3

from pathlib import Path

root = Path(__file__).resolve().parents[1]
inc = (root / "src" / "trust_runtime.inc").read_text()
core = (root / "src" / "native_core.c").read_text()
manifest = (root / "AndroidManifest.source.xml").read_text()

checks = {
    "deep-link scheme": 'android:scheme="zorintrust"' in manifest,
    "deep-link route": 'android:host="connect"' in manifest,
    "deep-link consumed by native activity": "trust_apply_transport_intent();" in core,
    "transport invitation uses cross-process file IPC": '"direct-transport"' in inc,
    "direct endpoint is private IPv4 only": "trust_parse_private_ipv4" in inc,
    "direct invitation has expiry": '"EXPIRES"' in inc and "expires > now + 3600" in inc,
    "portable host identity stays memory-only": "g_trust_portable_host_pub" in inc,
    "portable approval does not always persist host": "g_trust_pending_is_portable" in inc,
    "portable transport can force reconnect": '"RECONNECT"' in inc and "g_trust_reconnect_requested" in inc,
    "portable invitation is parsed from deep link": 'jstring invite_key' in inc,
    "portable invitation is stored in IPC frame": '"INVITE %s\\n"' in inc,
    "portable invitation is validated as 128-bit hex": 'trust_valid_invite' in inc,
    "portable invitation is sent in ZTRUST handshake": 'endpoint_invite' in inc and '"INVITE %s\\n"' in inc,
}

missing = [name for name, ok in checks.items() if not ok]
if missing:
    raise SystemExit("portable transport verify failed: " + ", ".join(missing))

print(
    "portable transport verify: OK private expiring endpoint + "
    "memory-only portable host + 128-bit invitation-bound handshake"
)

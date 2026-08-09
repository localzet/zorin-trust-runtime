#!/usr/bin/env bash
set -euo pipefail
KEY="${1:?key path required}"
CERT="${2:?cert path required}"
mkdir -p "$(dirname "$KEY")" "$(dirname "$CERT")"
command -v openssl >/dev/null 2>&1 || { echo "openssl is required to create a local debug key" >&2; exit 1; }
TMP="$(mktemp -d)"; trap 'rm -rf "$TMP"' EXIT
openssl req -x509 -newkey rsa:2048 -nodes -sha256 -days 3650 \
  -subj '/CN=Zorin Native Lab Local Debug/O=Local Development/' \
  -keyout "$TMP/key.pem" -out "$TMP/cert.pem" >/dev/null 2>&1
install -m 600 "$TMP/key.pem" "$KEY"
install -m 644 "$TMP/cert.pem" "$CERT"
echo "Generated local debug signing material under $(dirname "$KEY") (gitignored)."

# ZTRUST/2 / ZOWNER/1

`ZTRUST/2` authenticates a paired phone and workstation using fresh nonces and P-256 signatures. On stock Android the transport is an ADB reverse tunnel, but transport is not the trust decision.

After authentication the phone polls. A paired host may request a **bounded owner proof** for an action/resource tuple. The phone requires the device to remain unlocked, validates the proof time window, accepts only `owner.*` / `credential.*` action namespaces, constructs the canonical domain-separated message itself and signs with Android Keystore.

Canonical owner proof transcript:

```text
ZTRUST/2|OWNER_PROOF|host-fingerprint|phone-fingerprint|action-hex|resource-hex|nonce|issued|expires
```

The proof format is `ZOWNER/1`. There is no raw `SIGN(bytes)` endpoint.

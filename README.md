# Zorin Trust Runtime

## Runtime 8.2.0 — portable transport foundation

Android half of Zorin Trust. The runtime remains native-first: a NativeActivity UI plus a separate `:trust` foreground process, `ZTRUST/2` mutual authentication, bounded `ZOWNER/1` owner proofs and app-private `ZTRUSTUI/1` cross-process approval binding.

Runtime 8.2.0 adds an expiring `zorintrust://connect` deep link for direct private-LAN transport. Portable invitations include a random 128-bit token that is copied into the first ZTRUST/2 client frame, preventing an unrelated LAN scanner from consuming the one-shot host pairing window. A portable workstation is still authenticated by the normal ZTRUST/2 transcript, but its `portable/ephemeral` host key is pinned only in memory and is not written to the persistent trusted-host registry.

The stock owner-workstation path remains ADB/USB and is fully compatible with the existing owner-managed APK signing line.

See `docs/TRUST-VISUAL-CHANNEL.md`, `docs/TRUST-SERVICE.md`, and `docs/SECURITY-MODEL.md`.

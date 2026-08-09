# Signing

Release signing material is **not committed**. Git/local builds create a gitignored debug key under `app-native/.local/signing/`.

Official v0.3.2+ releases in this development line are signed by certificate SHA-256:

```text
d574e2ccaabacd87f5511a1f6d34ce23607564c6ee5690e3e1910e0b75797f28
```

The v0.3.0 APK was accidentally signed with a transient local key that was not retained. Because Android requires signer continuity for package updates, moving from the broken v0.3.0 build to v0.3.2 requires a one-time uninstall/reinstall and phone re-pair. Future releases must retain the certificate above.

Never commit release private keys.

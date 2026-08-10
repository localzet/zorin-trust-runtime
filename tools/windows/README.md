# Windows owner-managed APK signing

Development bundles after Runtime 5.0.3 use an **owner-managed signing key** stored outside the repository under `%LOCALAPPDATA%\\ZorinTrust\\signing`.

Why: Android requires signer continuity for updates, while release private keys must not be committed or embedded in public artifacts. Keeping the pre-release signer on the owner's workstation makes local update continuity independent of CI/chat build workspaces.

`runtime-signing.ps1` locates Android SDK Build-Tools `apksigner`, creates a local PKCS#12 RSA key when needed, and signs an unsigned Runtime APK. The key and password file must never be committed.

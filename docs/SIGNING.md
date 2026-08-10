# Signing

Repository builds create gitignored local debug signing material under `app-native/.local/signing/` unless explicit signing paths are supplied.

For the Windows owner bundle, Runtime 5.0.4 moves **pre-release update signing ownership to the owner's laptop**:

- persistent key: `%LOCALAPPDATA%\ZorinTrust\signing\runtime-owner.p12`
- password file: `%LOCALAPPDATA%\ZorinTrust\signing\runtime-owner.pass`
- both are outside the Git checkout and must remain private;
- Android SDK Build-Tools `apksigner` signs the distributed unsigned APK locally;
- future bundles reuse the same local key, preserving Android package update continuity.

The 5.0.3 assistant-side signer private key was not retained outside its build workspace. Updating from that build therefore requires one final clean reinstall and phone re-pair. This migration is intentionally the last build-workspace-dependent signer transition.

Never commit APK signing private keys, Android Keystore identities, host identity keys, trust session tokens, or broker credentials.

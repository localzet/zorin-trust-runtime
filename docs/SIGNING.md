# Signing

Release signing material is not committed. Local builds create a gitignored key under `app-native/.local/signing/`.

APK updates require the same signing certificate. Losing the release private key forces an uninstall/reinstall and may invalidate app-scoped Keystore state. Keep production release keys in a proper external secret store.

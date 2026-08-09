#!/usr/bin/env bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
BUILD="$HERE/build"
JAVAC_BIN="$(command -v javac || true)"
if [[ -z "$JAVAC_BIN" ]]; then echo "javac/JDK is required for JNI headers" >&2; exit 1; fi
JAVA_HOME_RESOLVED="$(dirname "$(dirname "$(readlink -f "$JAVAC_BIN")")")"
JNI_OS_DIR="linux"
[[ -d "$JAVA_HOME_RESOLVED/include/$JNI_OS_DIR" ]] || { echo "JNI platform headers not found under $JAVA_HOME_RESOLVED/include" >&2; exit 1; }
rm -rf "$BUILD/stubs" "$BUILD/lib" "$BUILD/apkroot"
mkdir -p "$BUILD/stubs" "$BUILD/lib" "$BUILD/apkroot/lib"

# These tiny libraries are LINK-TIME STUBS ONLY. They are not shipped in the APK.
# Their SONAMEs cause the final libraries to depend on Android's real libc.so/libandroid.so.
declare -A TARGETS=(
  [arm64-v8a]="aarch64-linux-android29"
  [armeabi-v7a]="armv7a-linux-androideabi29"
  [x86_64]="x86_64-linux-android29"
  [x86]="i686-linux-android29"
)

for ABI in "${!TARGETS[@]}"; do
  TARGET="${TARGETS[$ABI]}"
  mkdir -p "$BUILD/stubs/$ABI" "$BUILD/lib/$ABI" "$BUILD/apkroot/lib/$ABI"
  clang -target "$TARGET" -nostdlib -fuse-ld=lld -shared \
    "$HERE/src/stub_libandroid.c" -Wl,-soname,libandroid.so -o "$BUILD/stubs/$ABI/libandroid.so"
  clang -target "$TARGET" -nostdlib -fuse-ld=lld -shared \
    "$HERE/src/stub_libc.c" -Wl,-soname,libc.so -o "$BUILD/stubs/$ABI/libc.so"
  clang -target "$TARGET" -nostdlib -fuse-ld=lld -shared \
    "$HERE/src/stub_libbinder_ndk.c" -Wl,-soname,libbinder_ndk.so -o "$BUILD/stubs/$ABI/libbinder_ndk.so"

  clang -target "$TARGET" -O2 -fPIC -fvisibility=hidden -fno-stack-protector \
    -nostdlib -fuse-ld=lld -shared -I"$HERE/include" -I"$JAVA_HOME_RESOLVED/include" -I"$JAVA_HOME_RESOLVED/include/$JNI_OS_DIR" "$HERE/src/native_core.c" \
    -L"$BUILD/stubs/$ABI" -Wl,--no-as-needed -l:libandroid.so -l:libbinder_ndk.so -l:libc.so \
    -Wl,-soname,libzorin_native_core.so -Wl,--build-id=sha1 -Wl,--gc-sections \
    -o "$BUILD/lib/$ABI/libzorin_native_core.so"
  cp "$BUILD/lib/$ABI/libzorin_native_core.so" "$BUILD/apkroot/lib/$ABI/"
done
python3 "$HERE/tools/build_manifest.py" "$BUILD/apkroot/AndroidManifest.xml"
python3 "$HERE/tools/build_dex.py" "$BUILD/apkroot/classes.dex"
(
  cd "$BUILD/apkroot"
  zip -q -r "$BUILD/zorin-native-core-unsigned.apk" AndroidManifest.xml classes.dex lib
)

# Sign with APK Signature Scheme v2. The local Python signer follows the AOSP v2
# block/digest format. Public source generates a local key under .local/ unless explicit signing paths are supplied.
# Release signing material is intentionally not committed.
KEY="${ZORIN_SIGNING_KEY:-$HERE/.local/signing/debug-signing-key.pem}"
CERT="${ZORIN_SIGNING_CERT:-$HERE/.local/signing/debug-signing-cert.pem}"
if [[ ! -f "$KEY" || ! -f "$CERT" ]]; then
  "$HERE/tools/ensure_debug_key.sh" "$KEY" "$CERT"
fi
python3 "$HERE/tools/apk_v2.py" sign \
  "$BUILD/zorin-native-core-unsigned.apk" \
  "$BUILD/zorin-trust-runtime-v5.0.2.apk" \
  --key "$KEY" --cert "$CERT"

# If official Android Build Tools are installed, also verify with apksigner.
if command -v apksigner >/dev/null 2>&1; then
  apksigner verify --verbose --print-certs "$BUILD/zorin-trust-runtime-v5.0.2.apk"
fi

python3 "$HERE/tools/apk_v2.py" verify "$BUILD/zorin-trust-runtime-v5.0.2.apk"
sha256sum "$BUILD/zorin-trust-runtime-v5.0.2.apk"

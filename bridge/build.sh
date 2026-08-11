#!/usr/bin/env bash
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
BUILD="$HERE/build"

rm -rf "$BUILD"
mkdir -p "$BUILD/stubs"

declare -A TARGETS=(
  [arm64-v8a]="aarch64-linux-android29"
  [armeabi-v7a]="armv7a-linux-androideabi29"
  [x86_64]="x86_64-linux-android29"
  [x86]="i686-linux-android29"
)

declare -A STARTS=(
  [arm64-v8a]="start_aarch64.S"
  [armeabi-v7a]="start_arm.S"
  [x86_64]="start_x86_64.S"
  [x86]="start_x86.S"
)

declare -A LINKERS=(
  [arm64-v8a]="/system/bin/linker64"
  [armeabi-v7a]="/system/bin/linker"
  [x86_64]="/system/bin/linker64"
  [x86]="/system/bin/linker"
)

for ABI in "${!TARGETS[@]}"; do
  TARGET="${TARGETS[$ABI]}"
  mkdir -p "$BUILD/stubs/$ABI"

  clang \
    -target "$TARGET" \
    -nostdlib \
    -fuse-ld=lld \
    -shared \
    "$HERE/src/stub_libc.c" \
    -Wl,-soname,libc.so \
    -o "$BUILD/stubs/$ABI/libc.so"

  clang \
    -target "$TARGET" \
    -nostdlib \
    -fuse-ld=lld \
    -shared \
    "$HERE/src/stub_libbinder_ndk.c" \
    -Wl,-soname,libbinder_ndk.so \
    -o "$BUILD/stubs/$ABI/libbinder_ndk.so"

  clang \
    -target "$TARGET" \
    -O2 \
    -fPIC \
    -fno-stack-protector \
    -nostdlib \
    -fuse-ld=lld \
    -pie \
    "$HERE/src/agent.c" \
    "$HERE/src/${STARTS[$ABI]}" \
    -L"$BUILD/stubs/$ABI" \
    -Wl,--no-as-needed \
    -l:libbinder_ndk.so \
    -l:libc.so \
    -Wl,-e,_start \
    -Wl,--dynamic-linker,"${LINKERS[$ABI]}" \
    -Wl,--build-id=sha1 \
    -Wl,--gc-sections \
    -o "$BUILD/zorin-adb-core-$ABI"
done

sha256sum "$BUILD"/zorin-adb-core-*

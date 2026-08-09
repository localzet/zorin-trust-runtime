# Tiny DEX lifecycle shim

`TrustService` is generated without Gradle/D8 to keep the native-first build self-contained.

Runtime 5.0 had a format-35c encoder bug: the `A` argument-count nibble and `G` register nibble were reversed. ART correctly rejected the class. Runtime 5.0.1 fixes the encoding and runs an encoder regression test before emitting `classes.dex`.

The DEX contains lifecycle glue only; trust protocol, crypto, transport, UI rendering, and Keystore integration remain native.

# Contributing

Keep changes aligned with the trust model. New privileged backends must expose bounded capability APIs rather than generic execution primitives. Do not commit keys, tokens, local signing material or host pairing state.

For Android changes, preserve the no-DEX build unless a future feature has a strong documented reason to introduce application bytecode.

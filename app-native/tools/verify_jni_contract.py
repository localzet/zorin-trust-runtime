#!/usr/bin/env python3

import re
import sys
from pathlib import Path


source = Path(sys.argv[1]).read_text()

# Регрессия из native crash 5.0.3: jobject WindowManager нельзя передавать
# в GetMethodID; нужен jclass, который вернул GetObjectClass.
wrong_lookup = re.compile(
    r"GetMethodID\s*\(\s*env\s*,\s*wm\s*,\s*\"addView\"",
    re.MULTILINE,
)
correct_lookup = re.compile(
    r"GetMethodID\s*\(\s*env\s*,\s*wmc\s*,\s*\"addView\"",
    re.MULTILINE,
)

if wrong_lookup.search(source):
    raise SystemExit(
        "FAIL: WindowManager jobject passed to GetMethodID "
        "(5.0.3 crash regression)"
    )

if not correct_lookup.search(source):
    raise SystemExit(
        "FAIL: expected WindowManager class addView lookup missing"
    )

print("JNI contract verify: OK WindowManager addView lookup uses jclass")

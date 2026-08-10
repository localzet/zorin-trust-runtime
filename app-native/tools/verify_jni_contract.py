#!/usr/bin/env python3
from pathlib import Path
import re,sys
s=Path(sys.argv[1]).read_text()
# Regression for the 5.0.3 native crash: jobject WindowManager must never be
# passed to GetMethodID; the jclass returned by GetObjectClass is required.
if 'GetMethodID(env,wm,"addView"' in s:
    raise SystemExit('FAIL: WindowManager jobject passed to GetMethodID (5.0.3 crash regression)')
if 'GetMethodID(env,wmc,"addView"' not in s:
    raise SystemExit('FAIL: expected WindowManager class addView lookup missing')
print('JNI contract verify: OK WindowManager addView lookup uses jclass')

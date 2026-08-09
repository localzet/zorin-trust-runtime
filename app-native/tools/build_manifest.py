#!/usr/bin/env python3
import struct
from pathlib import Path

NO = 0xFFFFFFFF
RES_XML_TYPE = 0x0003
RES_STRING_POOL_TYPE = 0x0001
RES_XML_RESOURCE_MAP_TYPE = 0x0180
RES_XML_START_NAMESPACE_TYPE = 0x0100
RES_XML_END_NAMESPACE_TYPE = 0x0101
RES_XML_START_ELEMENT_TYPE = 0x0102
RES_XML_END_ELEMENT_TYPE = 0x0103
UTF8_FLAG = 0x00000100
TYPE_STRING = 0x03
TYPE_INT_DEC = 0x10
TYPE_INT_BOOLEAN = 0x12

# Stable framework resource IDs used by PackageParser/Resources.
RESOURCE_ATTRS = [
    ("label",            0x01010001),
    ("name",             0x01010003),
    ("hasCode",          0x0101000C),
    ("exported",         0x01010010),
    ("value",            0x01010024),
    ("minSdkVersion",    0x0101020C),
    ("versionCode",      0x0101021B),
    ("versionName",      0x0101021C),
    ("targetSdkVersion", 0x01010270),
]

strings = [x[0] for x in RESOURCE_ATTRS] + [
    "package", "android", "http://schemas.android.com/apk/res/android",
    "manifest", "uses-sdk", "application", "activity", "meta-data",
    "intent-filter", "action", "category", "uses-permission", "android.permission.INTERNET",
    "dev.zorin.nativelab", "3.0.0", "Zorin Trust Runtime",
    "android.app.NativeActivity", "android.app.lib_name", "zorin_native_core",
    "android.intent.action.MAIN", "android.intent.category.LAUNCHER",
]
idx = {s:i for i,s in enumerate(strings)}
ANDROID_URI = idx["http://schemas.android.com/apk/res/android"]

def uleb8_len(n: int) -> bytes:
    # Android UTF-8 string pool uses a one- or two-byte length prefix.
    if n < 0x80:
        return bytes([n])
    return bytes([0x80 | (n >> 8), n & 0xff])

def chunk_header(t, hs, size):
    return struct.pack('<HHI', t, hs, size)

def string_pool():
    blob = bytearray()
    offs = []
    for s in strings:
        b = s.encode('utf-8')
        offs.append(len(blob))
        # All strings here are ASCII, so UTF-16 code-unit len == UTF-8 byte len.
        blob += uleb8_len(len(s)) + uleb8_len(len(b)) + b + b'\0'
    while len(blob) % 4:
        blob += b'\0'
    header_size = 28
    strings_start = header_size + 4*len(strings)
    size = strings_start + len(blob)
    out = bytearray(chunk_header(RES_STRING_POOL_TYPE, header_size, size))
    out += struct.pack('<IIIII', len(strings), 0, UTF8_FLAG, strings_start, 0)
    out += struct.pack('<' + 'I'*len(offs), *offs)
    out += blob
    return bytes(out)

def resource_map():
    ids = [r for _,r in RESOURCE_ATTRS]
    size = 8 + 4*len(ids)
    return chunk_header(RES_XML_RESOURCE_MAP_TYPE, 8, size) + struct.pack('<'+'I'*len(ids), *ids)

def node_header(t, size, line=1):
    return chunk_header(t, 16, size) + struct.pack('<II', line, NO)

def ns_chunk(start=True, line=1):
    t = RES_XML_START_NAMESPACE_TYPE if start else RES_XML_END_NAMESPACE_TYPE
    size = 24
    return node_header(t, size, line) + struct.pack('<II', idx['android'], ANDROID_URI)

def typed_string(s):
    si = idx[s]
    return si, struct.pack('<HBBI', 8, 0, TYPE_STRING, si)

def typed_int(v):
    return NO, struct.pack('<HBBI', 8, 0, TYPE_INT_DEC, v & 0xffffffff)

def typed_bool(v):
    return NO, struct.pack('<HBBI', 8, 0, TYPE_INT_BOOLEAN, 0xffffffff if v else 0)

def attr(name, value, android=True, kind='string'):
    ns = ANDROID_URI if android else NO
    ni = idx[name]
    if kind == 'string': raw, tv = typed_string(value)
    elif kind == 'int': raw, tv = typed_int(value)
    elif kind == 'bool': raw, tv = typed_bool(value)
    else: raise ValueError(kind)
    return struct.pack('<III', ns, ni, raw) + tv

def start_tag(tag, attrs=(), line=1):
    body = bytearray()
    for a in attrs:
        body += attr(*a)
    attr_ext = struct.pack('<IIHHHHHH', NO, idx[tag], 20, 20, len(attrs), 0, 0, 0)
    size = 16 + len(attr_ext) + len(body)
    return node_header(RES_XML_START_ELEMENT_TYPE, size, line) + attr_ext + body

def end_tag(tag, line=1):
    size = 24
    return node_header(RES_XML_END_ELEMENT_TYPE, size, line) + struct.pack('<II', NO, idx[tag])

def build():
    chunks = [string_pool(), resource_map(), ns_chunk(True,1)]
    chunks += [
        start_tag('manifest', [
            ('package', 'dev.zorin.nativelab', False, 'string'),
            ('versionCode', 30, True, 'int'),
            ('versionName', '3.0.0', True, 'string'),
        ], 2),
        start_tag('uses-sdk', [
            ('minSdkVersion', 29, True, 'int'),
            ('targetSdkVersion', 35, True, 'int'),
        ], 3),
        end_tag('uses-sdk', 3),
        start_tag('uses-permission', [('name', 'android.permission.INTERNET', True, 'string')], 4),
        end_tag('uses-permission', 4),
        start_tag('application', [
            ('label', 'Zorin Trust Runtime', True, 'string'),
            ('hasCode', False, True, 'bool'),
        ], 4),
        start_tag('activity', [
            ('name', 'android.app.NativeActivity', True, 'string'),
            ('exported', True, True, 'bool'),
        ], 5),
        start_tag('meta-data', [
            ('name', 'android.app.lib_name', True, 'string'),
            ('value', 'zorin_native_core', True, 'string'),
        ], 6),
        end_tag('meta-data', 6),
        start_tag('intent-filter', [], 7),
        start_tag('action', [('name', 'android.intent.action.MAIN', True, 'string')], 8),
        end_tag('action', 8),
        start_tag('category', [('name', 'android.intent.category.LAUNCHER', True, 'string')], 9),
        end_tag('category', 9),
        end_tag('intent-filter', 10),
        end_tag('activity', 11),
        end_tag('application', 12),
        end_tag('manifest', 13),
        ns_chunk(False, 14),
    ]
    payload = b''.join(chunks)
    return chunk_header(RES_XML_TYPE, 8, 8 + len(payload)) + payload

if __name__ == '__main__':
    import sys
    out = Path(sys.argv[1] if len(sys.argv) > 1 else 'AndroidManifest.xml')
    out.write_bytes(build())
    print(f'wrote {out} ({out.stat().st_size} bytes)')

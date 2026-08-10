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

# Stable framework attr IDs used by Android's manifest parser.
RESOURCE_ATTRS = [
    ("label",            0x01010001),
    ("name",             0x01010003),
    ("permission",       0x01010006),
    ("hasCode",          0x0101000C),
    ("exported",         0x01010010),
    ("process",          0x01010011),
    ("value",            0x01010024),
    ("minSdkVersion",    0x0101020C),
    ("versionCode",      0x0101021B),
    ("versionName",      0x0101021C),
    ("targetSdkVersion", 0x01010270),
    ("stopWithTask",     0x0101036A),
    ("foregroundServiceType", 0x01010599),
]

strings = [x[0] for x in RESOURCE_ATTRS] + [
    "package", "android", "http://schemas.android.com/apk/res/android",
    "manifest", "uses-sdk", "application", "activity", "service", "property", "meta-data",
    "intent-filter", "action", "category", "uses-permission",
    "android.permission.INTERNET",
    "android.permission.FOREGROUND_SERVICE",
    "android.permission.FOREGROUND_SERVICE_SPECIAL_USE",
    "android.permission.SYSTEM_ALERT_WINDOW",
    "dev.zorin.trustruntime", "7.0.0", "Zorin Trust",
    ":trust",
    "android.app.NativeActivity", "dev.zorin.trustruntime.TrustService",
    "android.permission.DUMP",
    "android.app.PROPERTY_SPECIAL_USE_FGS_SUBTYPE",
    "persistent cryptographic owner-device trust over authenticated USB/ADB transport",
    "android.app.lib_name", "zorin_native_core",
    "android.intent.action.MAIN", "android.intent.category.LAUNCHER",
]
idx = {s:i for i,s in enumerate(strings)}
ANDROID_URI = idx["http://schemas.android.com/apk/res/android"]

def uleb8_len(n: int) -> bytes:
    if n < 0x80: return bytes([n])
    return bytes([0x80 | (n >> 8), n & 0xff])

def chunk_header(t, hs, size): return struct.pack('<HHI', t, hs, size)

def string_pool():
    blob = bytearray(); offs = []
    for s in strings:
        b = s.encode('utf-8'); offs.append(len(blob))
        blob += uleb8_len(len(s)) + uleb8_len(len(b)) + b + b'\0'
    while len(blob) % 4: blob += b'\0'
    header_size = 28; strings_start = header_size + 4*len(strings); size = strings_start + len(blob)
    out = bytearray(chunk_header(RES_STRING_POOL_TYPE, header_size, size))
    out += struct.pack('<IIIII', len(strings), 0, UTF8_FLAG, strings_start, 0)
    out += struct.pack('<' + 'I'*len(offs), *offs); out += blob
    return bytes(out)

def resource_map():
    ids = [r for _,r in RESOURCE_ATTRS]
    return chunk_header(RES_XML_RESOURCE_MAP_TYPE, 8, 8+4*len(ids)) + struct.pack('<'+'I'*len(ids), *ids)

def node_header(t, size, line=1): return chunk_header(t, 16, size) + struct.pack('<II', line, NO)
def ns_chunk(start=True, line=1):
    t = RES_XML_START_NAMESPACE_TYPE if start else RES_XML_END_NAMESPACE_TYPE
    return node_header(t,24,line)+struct.pack('<II',idx['android'],ANDROID_URI)
def typed_string(s):
    si=idx[s]; return si, struct.pack('<HBBI',8,0,TYPE_STRING,si)
def typed_int(v): return NO,struct.pack('<HBBI',8,0,TYPE_INT_DEC,v&0xffffffff)
def typed_bool(v): return NO,struct.pack('<HBBI',8,0,TYPE_INT_BOOLEAN,0xffffffff if v else 0)
def attr(name,value,android=True,kind='string'):
    ns=ANDROID_URI if android else NO; ni=idx[name]
    raw,tv=typed_string(value) if kind=='string' else (typed_int(value) if kind=='int' else typed_bool(value))
    return struct.pack('<III',ns,ni,raw)+tv
def start_tag(tag,attrs=(),line=1):
    # aapt2 emits framework attributes in ascending resource-id order. Keep the
    # same invariant: Android's manifest TypedArray path is built around the
    # compiled resource IDs, and hand-authored AXML must not rely on source XML
    # ordering. Non-android attributes (for example manifest package=) stay first.
    rid_by_name = dict(RESOURCE_ATTRS)
    attrs = list(attrs)
    attrs.sort(key=lambda a: (1, rid_by_name.get(a[0], 0xffffffff)) if a[2] else (0, 0))
    body=b''.join(attr(*a) for a in attrs)
    ext=struct.pack('<IIHHHHHH',NO,idx[tag],20,20,len(attrs),0,0,0)
    size=16+len(ext)+len(body); return node_header(RES_XML_START_ELEMENT_TYPE,size,line)+ext+body
def end_tag(tag,line=1): return node_header(RES_XML_END_ELEMENT_TYPE,24,line)+struct.pack('<II',NO,idx[tag])

def build():
    c=[string_pool(),resource_map(),ns_chunk(True,1)]
    c += [
      start_tag('manifest', [('package','dev.zorin.trustruntime',False,'string'),('versionCode',70,True,'int'),('versionName','7.0.0',True,'string')],2),
      start_tag('uses-sdk',[('minSdkVersion',29,True,'int'),('targetSdkVersion',35,True,'int')],3),end_tag('uses-sdk',3),
    ]
    for line,perm in enumerate(('android.permission.INTERNET','android.permission.FOREGROUND_SERVICE','android.permission.FOREGROUND_SERVICE_SPECIAL_USE','android.permission.SYSTEM_ALERT_WINDOW'),4):
        c += [start_tag('uses-permission',[('name',perm,True,'string')],line),end_tag('uses-permission',line)]
    c += [
      start_tag('application',[('label','Zorin Trust',True,'string'),('hasCode',True,True,'bool')],8),
      start_tag('activity',[('name','android.app.NativeActivity',True,'string'),('exported',True,True,'bool')],9),
      start_tag('meta-data',[('name','android.app.lib_name',True,'string'),('value','zorin_native_core',True,'string')],10),end_tag('meta-data',10),
      start_tag('intent-filter',[],11),
      start_tag('action',[('name','android.intent.action.MAIN',True,'string')],12),end_tag('action',12),
      start_tag('category',[('name','android.intent.category.LAUNCHER',True,'string')],13),end_tag('category',13),
      end_tag('intent-filter',14),end_tag('activity',15),
      # Exported so adb-shell can bootstrap it without waking the UI. android.permission.DUMP
      # restricts that exported entry point to shell/system-level callers; same-UID app calls still work.
      start_tag('service',[
          ('name','dev.zorin.trustruntime.TrustService',True,'string'),
          ('exported',True,True,'bool'),
          ('permission','android.permission.DUMP',True,'string'),
          ('stopWithTask',False,True,'bool'),
          ('process',':trust',True,'string'),
          ('foregroundServiceType',0x40000000,True,'int'),
      ],16),
      start_tag('property',[('name','android.app.PROPERTY_SPECIAL_USE_FGS_SUBTYPE',True,'string'),('value','persistent cryptographic owner-device trust over authenticated USB/ADB transport',True,'string')],17),end_tag('property',17),
      end_tag('service',18),
      end_tag('application',19),end_tag('manifest',20),ns_chunk(False,21),
    ]
    payload=b''.join(c); return chunk_header(RES_XML_TYPE,8,8+len(payload))+payload

if __name__=='__main__':
    import sys
    out=Path(sys.argv[1] if len(sys.argv)>1 else 'AndroidManifest.xml');out.write_bytes(build());print(f'wrote {out} ({out.stat().st_size} bytes)')

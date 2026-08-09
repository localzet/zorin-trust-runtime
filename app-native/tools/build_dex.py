#!/usr/bin/env python3
"""Build the tiny lifecycle-only classes.dex used by Zorin Trust Runtime.

The app stays native-first: this file contains only dev.zorin.trustruntime.TrustService,
a minimal Android Service shim that loads libzorin_native_core.so and forwards lifecycle
callbacks into JNI.  Keeping this builder in-tree avoids a Gradle/D8 dependency for the
low-level reproducible build.
"""
from __future__ import annotations
import argparse, hashlib, struct, zlib
from dataclasses import dataclass
from pathlib import Path

NO_INDEX = 0xFFFFFFFF
DEX_MAGIC = b"dex\n035\0"
HEADER_SIZE = 0x70
ENDIAN_TAG = 0x12345678

ACC_PUBLIC = 0x0001
ACC_PRIVATE = 0x0002
ACC_STATIC = 0x0008
ACC_FINAL = 0x0010
ACC_NATIVE = 0x0100
ACC_CONSTRUCTOR = 0x10000

TYPE_HEADER_ITEM = 0x0000
TYPE_STRING_ID_ITEM = 0x0001
TYPE_TYPE_ID_ITEM = 0x0002
TYPE_PROTO_ID_ITEM = 0x0003
TYPE_METHOD_ID_ITEM = 0x0005
TYPE_CLASS_DEF_ITEM = 0x0006
TYPE_MAP_LIST = 0x1000
TYPE_TYPE_LIST = 0x1001
TYPE_CLASS_DATA_ITEM = 0x2000
TYPE_CODE_ITEM = 0x2001
TYPE_STRING_DATA_ITEM = 0x2002


def uleb(n: int) -> bytes:
    assert n >= 0
    out = bytearray()
    while True:
        b = n & 0x7F
        n >>= 7
        if n:
            out.append(b | 0x80)
        else:
            out.append(b)
            return bytes(out)


def align(buf: bytearray, n: int = 4) -> None:
    while len(buf) % n:
        buf.append(0)


def utf16_len(s: str) -> int:
    return len(s.encode("utf-16-le")) // 2


@dataclass(frozen=True)
class Proto:
    ret: str
    params: tuple[str, ...]

    @property
    def shorty(self) -> str:
        def c(t: str) -> str:
            return t if len(t) == 1 else "L"
        return c(self.ret) + "".join(c(p) for p in self.params)


@dataclass(frozen=True)
class Method:
    cls: str
    name: str
    proto: Proto


# One deliberately boring Java shim:
#
# final class TrustService extends Service {
#   static { System.loadLibrary("zorin_native_core"); }
#   private static native void nativeOnCreate(Service s);
#   private static native void nativeOnStart(Service s, Intent i);
#   private static native void nativeOnDestroy(Service s);
#   public TrustService() { super(); }
#   public void onCreate() { super.onCreate(); nativeOnCreate(this); }
#   public int onStartCommand(Intent i,int flags,int startId) { nativeOnStart(this,i); return START_STICKY; }
#   public IBinder onBind(Intent i) { return null; }
#   public void onDestroy() { nativeOnDestroy(this); super.onDestroy(); }
# }

C_TRUST = "Ldev/zorin/trustruntime/TrustService;"
C_SERVICE = "Landroid/app/Service;"
C_INTENT = "Landroid/content/Intent;"
C_IBINDER = "Landroid/os/IBinder;"
C_SYSTEM = "Ljava/lang/System;"
C_STRING = "Ljava/lang/String;"
V = "V"
I = "I"

P_VOID = Proto(V, ())
P_LOADLIB = Proto(V, (C_STRING,))
P_NCREATE = Proto(V, (C_SERVICE,))
P_NSTART = Proto(V, (C_SERVICE, C_INTENT))
P_ONSTART = Proto(I, (C_INTENT, I, I))
P_ONBIND = Proto(C_IBINDER, (C_INTENT,))

M_SYSTEM_LOAD = Method(C_SYSTEM, "loadLibrary", P_LOADLIB)
M_SERVICE_INIT = Method(C_SERVICE, "<init>", P_VOID)
M_SERVICE_CREATE = Method(C_SERVICE, "onCreate", P_VOID)
M_SERVICE_DESTROY = Method(C_SERVICE, "onDestroy", P_VOID)
M_CLINIT = Method(C_TRUST, "<clinit>", P_VOID)
M_INIT = Method(C_TRUST, "<init>", P_VOID)
M_NCREATE = Method(C_TRUST, "nativeOnCreate", P_NCREATE)
M_NSTART = Method(C_TRUST, "nativeOnStart", P_NSTART)
M_NDESTROY = Method(C_TRUST, "nativeOnDestroy", P_NCREATE)
M_CREATE = Method(C_TRUST, "onCreate", P_VOID)
M_START = Method(C_TRUST, "onStartCommand", P_ONSTART)
M_BIND = Method(C_TRUST, "onBind", P_ONBIND)
M_DESTROY = Method(C_TRUST, "onDestroy", P_VOID)

ALL_METHODS = {
    M_SYSTEM_LOAD, M_SERVICE_INIT, M_SERVICE_CREATE, M_SERVICE_DESTROY,
    M_CLINIT, M_INIT, M_NCREATE, M_NSTART, M_NDESTROY,
    M_CREATE, M_START, M_BIND, M_DESTROY,
}
ALL_PROTOS = {m.proto for m in ALL_METHODS}
ALL_TYPES = {
    C_TRUST, C_SERVICE, C_INTENT, C_IBINDER, C_SYSTEM, C_STRING, V, I
} | {p.ret for p in ALL_PROTOS} | {x for p in ALL_PROTOS for x in p.params}

# Every name/descriptor/shorty plus the one const-string payload.
STRING_SET = set(ALL_TYPES) | {m.name for m in ALL_METHODS} | {p.shorty for p in ALL_PROTOS} | {"zorin_native_core"}
STRINGS = sorted(STRING_SET)
S = {v: i for i, v in enumerate(STRINGS)}
TYPES = sorted(ALL_TYPES, key=lambda x: S[x])
T = {v: i for i, v in enumerate(TYPES)}

# Proto ordering required by DEX: return type then argument type sequence.
PROTOS = sorted(ALL_PROTOS, key=lambda p: (T[p.ret], tuple(T[x] for x in p.params)))
P = {v: i for i, v in enumerate(PROTOS)}
METHODS = sorted(ALL_METHODS, key=lambda m: (T[m.cls], S[m.name], P[m.proto]))
M = {v: i for i, v in enumerate(METHODS)}


def ins_const_string(reg: int, string_idx: int) -> list[int]:
    assert 0 <= reg < 256 and string_idx < 65536
    return [0x001A | (reg << 8), string_idx]


def ins_const4(reg: int, lit: int) -> list[int]:
    # format 11n: op | A<<8 | (lit4&0xf)<<12
    return [0x0012 | ((reg & 0xF) << 8) | ((lit & 0xF) << 12)]


def ins_invoke(op: int, method_idx: int, regs: list[int]) -> list[int]:
    # format 35c: A=count, G in high nibble of first unit; C,D,E,F in third unit.
    assert len(regs) <= 5 and method_idx < 65536
    rr = regs + [0] * (5 - len(regs))
    c, d, e, f, g = rr[:5]
    # DEX format 35c packs G in bits 8..11 and A (argument count) in bits 12..15.
    # v0.3 accidentally swapped these fields, so ART decoded every invoke as A=0.
    first = op | ((g & 0xF) << 8) | (len(regs) << 12)
    third = (c & 0xF) | ((d & 0xF) << 4) | ((e & 0xF) << 8) | ((f & 0xF) << 12)
    return [first, method_idx, third]


def code_item(registers: int, ins: int, outs: int, units: list[int]) -> bytes:
    return struct.pack("<HHHHII", registers, ins, outs, 0, 0, len(units)) + struct.pack("<" + "H" * len(units), *units)


def build(out_path: Path) -> None:
    validate_invoke_encoder()
    # ---- fixed-size ID sections ----
    string_ids_off = HEADER_SIZE
    type_ids_off = string_ids_off + 4 * len(STRINGS)
    proto_ids_off = type_ids_off + 4 * len(TYPES)
    method_ids_off = proto_ids_off + 12 * len(PROTOS)
    class_defs_off = method_ids_off + 8 * len(METHODS)
    data_off = class_defs_off + 32
    assert data_off % 4 == 0

    data = bytearray()

    # type_list items, deduplicated by exact parameter tuple.
    type_list_off: dict[tuple[str, ...], int] = {}
    type_list_items = []
    for p in PROTOS:
        if not p.params or p.params in type_list_off:
            continue
        align(data, 4)
        off = data_off + len(data)
        type_list_off[p.params] = off
        start = len(data)
        data += struct.pack("<I", len(p.params))
        for x in p.params:
            data += struct.pack("<H", T[x])
        align(data, 4)
        type_list_items.append((off, len(data) - start))

    # Code items. Only non-native methods have code.
    code_spec: dict[Method, tuple[int, int, int, list[int]]] = {
        M_CLINIT: (1, 0, 1,
            ins_const_string(0, S["zorin_native_core"]) +
            ins_invoke(0x71, M[M_SYSTEM_LOAD], [0]) + [0x000E]),
        M_INIT: (1, 1, 1, ins_invoke(0x70, M[M_SERVICE_INIT], [0]) + [0x000E]),
        M_CREATE: (1, 1, 1,
            ins_invoke(0x6F, M[M_SERVICE_CREATE], [0]) +
            ins_invoke(0x71, M[M_NCREATE], [0]) + [0x000E]),
        M_START: (4, 4, 2,
            ins_invoke(0x71, M[M_NSTART], [0, 1]) +
            ins_const4(0, 1) + [0x000F]),
        M_BIND: (2, 2, 0, ins_const4(0, 0) + [0x0011]),
        M_DESTROY: (1, 1, 1,
            ins_invoke(0x71, M[M_NDESTROY], [0]) +
            ins_invoke(0x6F, M[M_SERVICE_DESTROY], [0]) + [0x000E]),
    }
    code_off: dict[Method, int] = {}
    code_offsets = []
    for m in sorted(code_spec, key=lambda m: M[m]):
        align(data, 4)
        off = data_off + len(data)
        code_off[m] = off
        regs, ins, outs, units = code_spec[m]
        data += code_item(regs, ins, outs, units)
        code_offsets.append(off)

    # class_data_item; encoded methods must be method_idx sorted inside each group.
    direct = [
        (M_CLINIT, ACC_STATIC | ACC_CONSTRUCTOR),
        (M_INIT, ACC_PUBLIC | ACC_CONSTRUCTOR),
        (M_NCREATE, ACC_PRIVATE | ACC_STATIC | ACC_NATIVE),
        (M_NSTART, ACC_PRIVATE | ACC_STATIC | ACC_NATIVE),
        (M_NDESTROY, ACC_PRIVATE | ACC_STATIC | ACC_NATIVE),
    ]
    virtual = [
        (M_CREATE, ACC_PUBLIC), (M_START, ACC_PUBLIC), (M_BIND, ACC_PUBLIC), (M_DESTROY, ACC_PUBLIC),
    ]
    direct.sort(key=lambda x: M[x[0]])
    virtual.sort(key=lambda x: M[x[0]])
    class_data_off = data_off + len(data)
    cd = bytearray(uleb(0) + uleb(0) + uleb(len(direct)) + uleb(len(virtual)))
    for group in (direct, virtual):
        prev = 0
        first = True
        for m, flags in group:
            idx = M[m]
            diff = idx if first else idx - prev
            first = False
            prev = idx
            cd += uleb(diff) + uleb(flags) + uleb(code_off.get(m, 0))
    data += cd

    # String data items.
    string_data_offs = []
    string_data_first = data_off + len(data)
    for s in STRINGS:
        off = data_off + len(data)
        string_data_offs.append(off)
        b = s.encode("utf-8")  # all our strings are ASCII
        data += uleb(utf16_len(s)) + b + b"\0"

    # map_list must be 4-byte aligned and includes itself.
    align(data, 4)
    map_off = data_off + len(data)
    map_entries = [
        (TYPE_HEADER_ITEM, 1, 0),
        (TYPE_STRING_ID_ITEM, len(STRINGS), string_ids_off),
        (TYPE_TYPE_ID_ITEM, len(TYPES), type_ids_off),
        (TYPE_PROTO_ID_ITEM, len(PROTOS), proto_ids_off),
        (TYPE_METHOD_ID_ITEM, len(METHODS), method_ids_off),
        (TYPE_CLASS_DEF_ITEM, 1, class_defs_off),
    ]
    if type_list_items:
        map_entries.append((TYPE_TYPE_LIST, len(type_list_items), type_list_items[0][0]))
    map_entries += [
        (TYPE_CODE_ITEM, len(code_offsets), code_offsets[0]),
        (TYPE_CLASS_DATA_ITEM, 1, class_data_off),
        (TYPE_STRING_DATA_ITEM, len(STRINGS), string_data_first),
        (TYPE_MAP_LIST, 1, map_off),
    ]
    map_entries.sort(key=lambda x: x[2])
    data += struct.pack("<I", len(map_entries))
    for typ, size, off in map_entries:
        data += struct.pack("<HHII", typ, 0, size, off)

    data_size = len(data)
    file_size = data_off + data_size

    # Build fixed sections with now-known offsets.
    fixed = bytearray(b"\0" * HEADER_SIZE)
    for off in string_data_offs:
        fixed += struct.pack("<I", off)
    for t in TYPES:
        fixed += struct.pack("<I", S[t])
    for p in PROTOS:
        fixed += struct.pack("<III", S[p.shorty], T[p.ret], type_list_off.get(p.params, 0))
    for m in METHODS:
        fixed += struct.pack("<HHI", T[m.cls], P[m.proto], S[m.name])

    # Single TrustService class definition.
    fixed += struct.pack(
        "<IIIIIIII",
        T[C_TRUST], ACC_PUBLIC | ACC_FINAL, T[C_SERVICE], 0,
        NO_INDEX, 0, class_data_off, 0,
    )
    assert len(fixed) == data_off
    dex = fixed + data

    # Header (signature/checksum patched after all other bytes).
    struct.pack_into("8sI20s20I", dex, 0,
        DEX_MAGIC, 0, b"\0" * 20,
        file_size, HEADER_SIZE, ENDIAN_TAG, 0, 0, map_off,
        len(STRINGS), string_ids_off,
        len(TYPES), type_ids_off,
        len(PROTOS), proto_ids_off,
        0, 0,
        len(METHODS), method_ids_off,
        1, class_defs_off,
        data_size, data_off,
    )
    sig = hashlib.sha1(dex[32:]).digest()
    dex[12:32] = sig
    checksum = zlib.adler32(dex[12:]) & 0xFFFFFFFF
    struct.pack_into("<I", dex, 8, checksum)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(dex)
    validate(out_path)
    print(f"wrote {out_path} ({len(dex)} bytes), strings={len(STRINGS)} methods={len(METHODS)}")


def validate_invoke_encoder() -> None:
    # Catch the exact v0.3 regression: in format 35c the argument count lives
    # in the HIGH nibble of the first code unit, while G lives in bits 8..11.
    cases = [
        (0x71, 7, [0]),
        (0x71, 7, [0, 1]),
        (0x70, 7, [3]),
        (0x6F, 7, [2]),
        (0x71, 7, [0, 1, 2, 3, 4]),
    ]
    for op, midx, regs in cases:
        units = ins_invoke(op, midx, regs)
        first = units[0]
        decoded_op = first & 0xFF
        decoded_g = (first >> 8) & 0xF
        decoded_count = (first >> 12) & 0xF
        if decoded_op != op or decoded_count != len(regs):
            raise SystemExit(f"DEX 35c invoke encoding regression: op={op:#x} regs={regs} first={first:#06x}")
        expected_g = regs[4] if len(regs) == 5 else 0
        if decoded_g != expected_g:
            raise SystemExit(f"DEX 35c G-register encoding regression: regs={regs} got G={decoded_g}")


def validate(path: Path) -> None:
    b = path.read_bytes()
    if len(b) < HEADER_SIZE or b[:8] != DEX_MAGIC:
        raise SystemExit("invalid DEX magic/header")
    checksum, = struct.unpack_from("<I", b, 8)
    if checksum != (zlib.adler32(b[12:]) & 0xFFFFFFFF):
        raise SystemExit("DEX Adler32 mismatch")
    if b[12:32] != hashlib.sha1(b[32:]).digest():
        raise SystemExit("DEX SHA1 mismatch")
    file_size, header_size, endian = struct.unpack_from("<III", b, 32)
    if file_size != len(b) or header_size != HEADER_SIZE or endian != ENDIAN_TAG:
        raise SystemExit("DEX header fields invalid")
    # Ensure key class/method names are actually present as NUL-terminated data.
    for needle in (b"Ldev/zorin/trustruntime/TrustService;\0", b"nativeOnStart\0", b"zorin_native_core\0"):
        if needle not in b:
            raise SystemExit(f"DEX expected string missing: {needle!r}")


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("output", nargs="?", default="classes.dex")
    ns = ap.parse_args()
    build(Path(ns.output))

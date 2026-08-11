#!/usr/bin/env python3

"""Tiny APK Signature Scheme v2 signer/verifier for this no-Gradle demo.

Implements one signer using RSA PKCS#1 v1.5 + SHA-256 (algorithm 0x0103).
It intentionally does not implement v1/v3/v4. minSdk for this project is 24.
"""
from __future__ import annotations
import argparse, datetime, hashlib, os, struct, sys
from pathlib import Path
from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa
from cryptography.x509.oid import NameOID
V2_ID = 1896449818
ALG_RSA_PKCS1_SHA256 = 259
MAGIC = b'APK Sig Block 42'
ONE_MB = 1 << 20
EOCD_SIG = b'PK\x05\x06'

def u32(n: int) -> bytes:
    return struct.pack('<I', n)

def u64(n: int) -> bytes:
    return struct.pack('<Q', n)

def lp32(b: bytes) -> bytes:
    return u32(len(b)) + b

def find_eocd(data: bytes) -> tuple[int, int, int]:
    start = max(0, len(data) - (65535 + 22))
    pos = data.rfind(EOCD_SIG, start)
    while pos >= 0:
        if pos + 22 <= len(data):
            comment_len = struct.unpack_from('<H', data, pos + 20)[0]
            if pos + 22 + comment_len == len(data):
                cd_size = struct.unpack_from('<I', data, pos + 12)[0]
                cd_off = struct.unpack_from('<I', data, pos + 16)[0]
                if cd_off != 4294967295 and cd_size != 4294967295:
                    return (pos, cd_off, cd_size)
        pos = data.rfind(EOCD_SIG, start, pos)
    raise ValueError('ZIP EOCD not found or ZIP64 unsupported')

def content_digest(sections: list[bytes]) -> bytes:
    chunk_digests: list[bytes] = []
    for section in sections:
        for off in range(0, len(section), ONE_MB):
            chunk = section[off:off + ONE_MB]
            h = hashlib.sha256()
            h.update(b'\xa5')
            h.update(u32(len(chunk)))
            h.update(chunk)
            chunk_digests.append(h.digest())
    h = hashlib.sha256()
    h.update(b'Z')
    h.update(u32(len(chunk_digests)))
    for d in chunk_digests:
        h.update(d)
    return h.digest()

def make_debug_key(key_path: Path, cert_path: Path) -> None:
    if key_path.exists() and cert_path.exists():
        return
    key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    subject = issuer = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, 'Zorin Native Lab Debug'), x509.NameAttribute(NameOID.ORGANIZATION_NAME, 'Zorin Native Lab')])
    now = datetime.datetime.now(datetime.timezone.utc)
    cert = x509.CertificateBuilder().subject_name(subject).issuer_name(issuer).public_key(key.public_key()).serial_number(x509.random_serial_number()).not_valid_before(now - datetime.timedelta(days=1)).not_valid_after(now + datetime.timedelta(days=3650)).add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True).sign(key, hashes.SHA256())
    key_path.write_bytes(key.private_bytes(serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
    cert_path.write_bytes(cert.public_bytes(serialization.Encoding.PEM))

def build_v2_value(digest: bytes, cert_der: bytes, pub_der: bytes, key) -> bytes:
    digest_record = u32(ALG_RSA_PKCS1_SHA256) + lp32(digest)
    digests_field = lp32(lp32(digest_record))
    certs_field = lp32(lp32(cert_der))
    attrs_field = lp32(b'')
    signed_data = digests_field + certs_field + attrs_field
    signature = key.sign(signed_data, padding.PKCS1v15(), hashes.SHA256())
    sig_record = u32(ALG_RSA_PKCS1_SHA256) + lp32(signature)
    signer = lp32(signed_data) + lp32(lp32(sig_record)) + lp32(pub_der)
    return lp32(lp32(signer))

def build_signing_block(v2_value: bytes) -> bytes:
    pair = u64(4 + len(v2_value)) + u32(V2_ID) + v2_value
    size = len(pair) + 24
    return u64(size) + pair + u64(size) + MAGIC

def sign_apk(inp: Path, out: Path, key_path: Path, cert_path: Path) -> None:
    make_debug_key(key_path, cert_path)
    data = inp.read_bytes()
    eocd_off, cd_off, cd_size = find_eocd(data)
    if cd_off + cd_size != eocd_off:
        raise ValueError('Central directory is not immediately followed by EOCD')
    key = serialization.load_pem_private_key(key_path.read_bytes(), password=None)
    cert = x509.load_pem_x509_certificate(cert_path.read_bytes())
    cert_der = cert.public_bytes(serialization.Encoding.DER)
    pub_der = cert.public_key().public_bytes(serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo)
    digest = content_digest([data[:cd_off], data[cd_off:eocd_off], data[eocd_off:]])
    v2 = build_v2_value(digest, cert_der, pub_der, key)
    block = build_signing_block(v2)
    eocd = bytearray(data[eocd_off:])
    struct.pack_into('<I', eocd, 16, cd_off + len(block))
    signed = data[:cd_off] + block + data[cd_off:eocd_off] + bytes(eocd)
    out.write_bytes(signed)

def take_lp(buf: bytes, pos: int) -> tuple[bytes, int]:
    if pos + 4 > len(buf):
        raise ValueError('truncated lp32')
    n = struct.unpack_from('<I', buf, pos)[0]
    pos += 4
    if pos + n > len(buf):
        raise ValueError('truncated lp32 payload')
    return (buf[pos:pos + n], pos + n)

def parse_first_lp_sequence(buf: bytes) -> list[bytes]:
    seq, end = take_lp(buf, 0)
    if end != len(buf):
        raise ValueError('trailing bytes after sequence')
    items = []
    p = 0
    while p < len(seq):
        item, p = take_lp(seq, p)
        items.append(item)
    return items

def locate_v2(data: bytes):
    eocd_off, cd_off, cd_size = find_eocd(data)
    if cd_off + cd_size != eocd_off:
        raise ValueError('CD/EOCD adjacency invalid')
    if cd_off < 32 or data[cd_off - 16:cd_off] != MAGIC:
        raise ValueError('APK Signing Block magic missing')
    size2 = struct.unpack_from('<Q', data, cd_off - 24)[0]
    block_start = cd_off - (size2 + 8)
    if block_start < 0:
        raise ValueError('invalid APK Signing Block size')
    size1 = struct.unpack_from('<Q', data, block_start)[0]
    if size1 != size2:
        raise ValueError('APK Signing Block size mismatch')
    p = block_start + 8
    pairs_end = cd_off - 24
    v2 = None
    while p < pairs_end:
        if p + 8 > pairs_end:
            raise ValueError('truncated pair')
        n = struct.unpack_from('<Q', data, p)[0]
        p += 8
        if n < 4 or p + n > pairs_end:
            raise ValueError('bad pair length')
        pid = struct.unpack_from('<I', data, p)[0]
        val = data[p + 4:p + n]
        p += n
        if pid == V2_ID and v2 is None:
            v2 = val
    if p != pairs_end:
        raise ValueError('pair area misaligned')
    if v2 is None:
        raise ValueError('v2 pair missing')
    return (eocd_off, cd_off, cd_size, block_start, v2)

def verify_apk(path: Path) -> dict:
    data = path.read_bytes()
    eocd_off, cd_off, cd_size, block_start, v2 = locate_v2(data)
    signers = parse_first_lp_sequence(v2)
    if len(signers) != 1:
        raise ValueError(f'expected 1 signer, got {len(signers)}')
    signer = signers[0]
    p = 0
    signed_data, p = take_lp(signer, p)
    sigs_seq, p = take_lp(signer, p)
    pub_der, p = take_lp(signer, p)
    if p != len(signer):
        raise ValueError('trailing signer bytes')
    q = 0
    digests_seq, q = take_lp(signed_data, q)
    certs_seq, q = take_lp(signed_data, q)
    attrs_seq, q = take_lp(signed_data, q)
    if q != len(signed_data) or attrs_seq:
        raise ValueError('bad signed-data layout')
    digest_records = []
    q = 0
    while q < len(digests_seq):
        rec, q = take_lp(digests_seq, q)
        digest_records.append(rec)
    certs = []
    q = 0
    while q < len(certs_seq):
        c, q = take_lp(certs_seq, q)
        certs.append(c)
    sig_records = []
    q = 0
    while q < len(sigs_seq):
        rec, q = take_lp(sigs_seq, q)
        sig_records.append(rec)
    if not digest_records or not sig_records or (not certs):
        raise ValueError('missing signer records')
    digest_by_alg = {}
    for dr in digest_records:
        alg = struct.unpack_from('<I', dr, 0)[0]
        dg, end = take_lp(dr, 4)
        if end != len(dr):
            raise ValueError('malformed digest record')
        digest_by_alg[alg] = dg
    sig_by_alg = {}
    for sr in sig_records:
        alg = struct.unpack_from('<I', sr, 0)[0]
        sg, end = take_lp(sr, 4)
        if end != len(sr):
            raise ValueError('malformed signature record')
        sig_by_alg[alg] = sg
    if list(digest_by_alg.keys()) != list(sig_by_alg.keys()):
        raise ValueError('digest/signature algorithm lists differ')
    alg = ALG_RSA_PKCS1_SHA256
    if alg not in digest_by_alg or alg not in sig_by_alg:
        raise ValueError('RSA PKCS1 SHA-256 record missing')
    stored_digest = digest_by_alg[alg]
    signature = sig_by_alg[alg]
    cert = x509.load_der_x509_certificate(certs[0])
    cert_pub = cert.public_key().public_bytes(serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo)
    if cert_pub != pub_der:
        raise ValueError('certificate/public-key mismatch')
    cert.public_key().verify(signature, signed_data, padding.PKCS1v15(), hashes.SHA256())
    eocd = bytearray(data[eocd_off:])
    struct.pack_into('<I', eocd, 16, block_start)
    actual = content_digest([data[:block_start], data[cd_off:eocd_off], bytes(eocd)])
    if actual != stored_digest:
        raise ValueError('APK content digest mismatch')
    return {'scheme': 'v2', 'algorithm': 'RSA-PKCS1-SHA256 (0x0103)', 'signers': 1, 'certificate_sha256': hashlib.sha256(certs[0]).hexdigest(), 'content_sha256_digest': stored_digest.hex(), 'apk_sha256': hashlib.sha256(data).hexdigest(), 'block_start': block_start, 'central_directory_offset': cd_off}

def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('sign')
    s.add_argument('input')
    s.add_argument('output')
    s.add_argument('--key', required=True)
    s.add_argument('--cert', required=True)
    v = sub.add_parser('verify')
    v.add_argument('apk')
    a = ap.parse_args()
    if a.cmd == 'sign':
        sign_apk(Path(a.input), Path(a.output), Path(a.key), Path(a.cert))
        info = verify_apk(Path(a.output))
        print('SIGNED+VERIFIED v2', info)
    else:
        print('VERIFIED', verify_apk(Path(a.apk)))
if __name__ == '__main__':
    main()

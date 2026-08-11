#!/usr/bin/env python3

import struct
import sys
from pathlib import Path

NO = 0xFFFFFFFF
RES_XML_TYPE = 0x0003
RES_STRING_POOL_TYPE = 0x0001
RES_XML_RESOURCE_MAP_TYPE = 0x0180
RES_XML_START_NAMESPACE_TYPE = 0x0100
RES_XML_END_NAMESPACE_TYPE = 0x0101
RES_XML_START_ELEMENT_TYPE = 0x0102
RES_XML_END_ELEMENT_TYPE = 0x0103
UTF8_FLAG = 0x0100
TYPE_STRING = 0x03
TYPE_INT_DEC = 0x10
TYPE_INT_BOOLEAN = 0x12

RESOURCE_ATTRS = [
    ('label', 0x01010001),
    ('name', 0x01010003),
    ('permission', 0x01010006),
    ('hasCode', 0x0101000C),
    ('exported', 0x01010010),
    ('process', 0x01010011),
    ('value', 0x01010024),
    ('scheme', 0x01010027),
    ('host', 0x01010028),
    ('minSdkVersion', 0x0101020C),
    ('versionCode', 0x0101021B),
    ('versionName', 0x0101021C),
    ('targetSdkVersion', 0x01010270),
    ('stopWithTask', 0x0101036A),
    ('foregroundServiceType', 0x01010519),
]

STRINGS = [
    *(name for name, _ in RESOURCE_ATTRS),
    'package',
    'android',
    'http://schemas.android.com/apk/res/android',
    'manifest',
    'uses-sdk',
    'application',
    'activity',
    'service',
    'property',
    'meta-data',
    'intent-filter',
    'action',
    'category',
    'data',
    'uses-permission',
    'android.permission.INTERNET',
    'android.permission.FOREGROUND_SERVICE',
    'android.permission.FOREGROUND_SERVICE_SPECIAL_USE',
    'android.permission.SYSTEM_ALERT_WINDOW',
    'dev.zorin.trustruntime',
    '8.2.0',
    'Zorin Trust',
    ':trust',
    'android.app.NativeActivity',
    'dev.zorin.trustruntime.TrustService',
    'android.permission.DUMP',
    'android.app.PROPERTY_SPECIAL_USE_FGS_SUBTYPE',
    'persistent cryptographic owner-device trust over authenticated transport',
    'android.app.lib_name',
    'zorin_native_core',
    'android.intent.action.MAIN',
    'android.intent.category.LAUNCHER',
    'android.intent.action.VIEW',
    'android.intent.category.DEFAULT',
    'android.intent.category.BROWSABLE',
    'zorintrust',
    'connect',
]

INDEX = {value: index for index, value in enumerate(STRINGS)}
ANDROID_URI = INDEX['http://schemas.android.com/apk/res/android']


def uleb8_len(value: int) -> bytes:
    if value < 128:
        return bytes([value])

    return bytes([0x80 | value >> 8, value & 0xFF])


def chunk_header(chunk_type: int, header_size: int, size: int) -> bytes:
    return struct.pack('<HHI', chunk_type, header_size, size)


def string_pool() -> bytes:
    blob = bytearray()
    offsets = []

    for value in STRINGS:
        encoded = value.encode('utf-8')
        offsets.append(len(blob))
        blob += uleb8_len(len(value))
        blob += uleb8_len(len(encoded))
        blob += encoded
        blob += b'\x00'

    while len(blob) % 4:
        blob += b'\x00'

    header_size = 28
    strings_start = header_size + 4 * len(STRINGS)
    size = strings_start + len(blob)

    output = bytearray(
        chunk_header(
            RES_STRING_POOL_TYPE,
            header_size,
            size,
        )
    )
    output += struct.pack(
        '<IIIII',
        len(STRINGS),
        0,
        UTF8_FLAG,
        strings_start,
        0,
    )
    output += struct.pack(
        '<' + 'I' * len(offsets),
        *offsets,
    )
    output += blob

    return bytes(output)


def resource_map() -> bytes:
    resource_ids = [resource_id for _, resource_id in RESOURCE_ATTRS]
    size = 8 + 4 * len(resource_ids)

    return chunk_header(
        RES_XML_RESOURCE_MAP_TYPE,
        8,
        size,
    ) + struct.pack(
        '<' + 'I' * len(resource_ids),
        *resource_ids,
    )


def node_header(chunk_type: int, size: int, line: int = 1) -> bytes:
    return chunk_header(
        chunk_type,
        16,
        size,
    ) + struct.pack(
        '<II',
        line,
        NO,
    )


def ns_chunk(start: bool = True, line: int = 1) -> bytes:
    chunk_type = (
        RES_XML_START_NAMESPACE_TYPE
        if start
        else RES_XML_END_NAMESPACE_TYPE
    )

    return node_header(
        chunk_type,
        24,
        line,
    ) + struct.pack(
        '<II',
        INDEX['android'],
        ANDROID_URI,
    )


def typed_string(value: str) -> tuple[int, bytes]:
    string_index = INDEX[value]

    return (
        string_index,
        struct.pack(
            '<HBBI',
            8,
            0,
            TYPE_STRING,
            string_index,
        ),
    )


def typed_int(value: int) -> tuple[int, bytes]:
    return (
        NO,
        struct.pack(
            '<HBBI',
            8,
            0,
            TYPE_INT_DEC,
            value & NO,
        ),
    )


def typed_bool(value: bool) -> tuple[int, bytes]:
    encoded = NO if value else 0

    return (
        NO,
        struct.pack(
            '<HBBI',
            8,
            0,
            TYPE_INT_BOOLEAN,
            encoded,
        ),
    )


def attr(
    name: str,
    value,
    android: bool = True,
    kind: str = 'string',
) -> bytes:
    namespace = ANDROID_URI if android else NO
    name_index = INDEX[name]

    if kind == 'string':
        raw, typed_value = typed_string(value)
    elif kind == 'int':
        raw, typed_value = typed_int(value)
    else:
        raw, typed_value = typed_bool(value)

    return struct.pack(
        '<III',
        namespace,
        name_index,
        raw,
    ) + typed_value


def start_tag(
    tag: str,
    attrs=(),
    line: int = 1,
) -> bytes:
    resource_id_by_name = dict(RESOURCE_ATTRS)
    attributes = list(attrs)

    # В бинарном AndroidManifest сначала идут обычные атрибуты, затем android:*.
    attributes.sort(
        key=lambda item: (
            (1, resource_id_by_name.get(item[0], NO))
            if item[2]
            else (0, 0)
        )
    )

    body = b''.join(attr(*item) for item in attributes)
    extension = struct.pack(
        '<IIHHHHHH',
        NO,
        INDEX[tag],
        20,
        20,
        len(attributes),
        0,
        0,
        0,
    )
    size = 16 + len(extension) + len(body)

    return (
        node_header(
            RES_XML_START_ELEMENT_TYPE,
            size,
            line,
        )
        + extension
        + body
    )


def end_tag(tag: str, line: int = 1) -> bytes:
    return node_header(
        RES_XML_END_ELEMENT_TYPE,
        24,
        line,
    ) + struct.pack(
        '<II',
        NO,
        INDEX[tag],
    )


def build() -> bytes:
    chunks = [
        string_pool(),
        resource_map(),
        ns_chunk(True, 1),
        start_tag(
            'manifest',
            [
                ('package', 'dev.zorin.trustruntime', False, 'string'),
                ('versionCode', 82, True, 'int'),
                ('versionName', '8.2.0', True, 'string'),
            ],
            2,
        ),
        start_tag(
            'uses-sdk',
            [
                ('minSdkVersion', 29, True, 'int'),
                ('targetSdkVersion', 35, True, 'int'),
            ],
            3,
        ),
        end_tag('uses-sdk', 3),
    ]

    permissions = (
        'android.permission.INTERNET',
        'android.permission.FOREGROUND_SERVICE',
        'android.permission.FOREGROUND_SERVICE_SPECIAL_USE',
        'android.permission.SYSTEM_ALERT_WINDOW',
    )

    for line, permission in enumerate(permissions, 4):
        chunks.extend(
            [
                start_tag(
                    'uses-permission',
                    [('name', permission, True, 'string')],
                    line,
                ),
                end_tag('uses-permission', line),
            ]
        )

    chunks.extend(
        [
            start_tag(
                'application',
                [
                    ('label', 'Zorin Trust', True, 'string'),
                    ('hasCode', True, True, 'bool'),
                ],
                8,
            ),
            start_tag(
                'activity',
                [
                    ('name', 'android.app.NativeActivity', True, 'string'),
                    ('exported', True, True, 'bool'),
                ],
                9,
            ),
            start_tag(
                'meta-data',
                [
                    ('name', 'android.app.lib_name', True, 'string'),
                    ('value', 'zorin_native_core', True, 'string'),
                ],
                10,
            ),
            end_tag('meta-data', 10),
            start_tag('intent-filter', [], 11),
            start_tag(
                'action',
                [('name', 'android.intent.action.MAIN', True, 'string')],
                12,
            ),
            end_tag('action', 12),
            start_tag(
                'category',
                [('name', 'android.intent.category.LAUNCHER', True, 'string')],
                13,
            ),
            end_tag('category', 13),
            end_tag('intent-filter', 14),
            start_tag('intent-filter', [], 15),
            start_tag(
                'action',
                [('name', 'android.intent.action.VIEW', True, 'string')],
                16,
            ),
            end_tag('action', 16),
            start_tag(
                'category',
                [('name', 'android.intent.category.DEFAULT', True, 'string')],
                17,
            ),
            end_tag('category', 17),
            start_tag(
                'category',
                [('name', 'android.intent.category.BROWSABLE', True, 'string')],
                18,
            ),
            end_tag('category', 18),
            start_tag(
                'data',
                [
                    ('scheme', 'zorintrust', True, 'string'),
                    ('host', 'connect', True, 'string'),
                ],
                19,
            ),
            end_tag('data', 19),
            end_tag('intent-filter', 20),
            end_tag('activity', 21),
            start_tag(
                'service',
                [
                    (
                        'name',
                        'dev.zorin.trustruntime.TrustService',
                        True,
                        'string',
                    ),
                    ('exported', True, True, 'bool'),
                    (
                        'permission',
                        'android.permission.DUMP',
                        True,
                        'string',
                    ),
                    ('stopWithTask', False, True, 'bool'),
                    ('process', ':trust', True, 'string'),
                    ('foregroundServiceType', 0x40000000, True, 'int'),
                ],
                22,
            ),
            start_tag(
                'property',
                [
                    (
                        'name',
                        'android.app.PROPERTY_SPECIAL_USE_FGS_SUBTYPE',
                        True,
                        'string',
                    ),
                    (
                        'value',
                        'persistent cryptographic owner-device trust over '
                        'authenticated transport',
                        True,
                        'string',
                    ),
                ],
                23,
            ),
            end_tag('property', 23),
            end_tag('service', 24),
            end_tag('application', 25),
            end_tag('manifest', 26),
            ns_chunk(False, 27),
        ]
    )

    payload = b''.join(chunks)

    return chunk_header(
        RES_XML_TYPE,
        8,
        8 + len(payload),
    ) + payload


def main() -> None:
    output = Path(
        sys.argv[1]
        if len(sys.argv) > 1
        else 'AndroidManifest.xml'
    )
    output.write_bytes(build())
    print(f'wrote {output} ({output.stat().st_size} bytes)')


if __name__ == '__main__':
    main()

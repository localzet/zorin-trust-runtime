#!/usr/bin/env python3

import struct
import sys
from pathlib import Path

NO_INDEX = 0xFFFFFFFF
STRING_POOL = 0x0001
RESOURCE_MAP = 0x0180
START_ELEMENT = 0x0102


def read_utf8_length(buffer: bytes, position: int) -> tuple[int, int]:
    first = buffer[position]
    if first < 128:
        return first, position + 1
    return ((first & 0x7F) << 8) | buffer[position + 1], position + 2


def parse_manifest(path: Path) -> list[tuple[str, list[tuple[str, int | None, object, int]]]]:
    data = path.read_bytes()
    position = 8
    strings: list[str] = []
    resource_ids: list[int] = []
    elements: list[tuple[str, list[tuple[str, int | None, object, int]]]] = []

    while position < len(data):
        chunk_type, header_size, chunk_size = struct.unpack_from('<HHI', data, position)

        if chunk_type == STRING_POOL:
            string_count, _, _, strings_start, _ = struct.unpack_from('<IIIII', data, position + 8)
            offsets = struct.unpack_from('<' + 'I' * string_count, data, position + header_size)
            base = position + strings_start

            for offset in offsets:
                cursor = base + offset
                _, cursor = read_utf8_length(data, cursor)
                byte_length, cursor = read_utf8_length(data, cursor)
                strings.append(data[cursor:cursor + byte_length].decode())

        elif chunk_type == RESOURCE_MAP:
            count = (chunk_size - 8) // 4
            resource_ids = list(struct.unpack_from('<' + 'I' * count, data, position + 8))

        elif chunk_type == START_ELEMENT:
            _, name_index = struct.unpack_from('<II', data, position + 16)
            attribute_start, attribute_size, attribute_count, _, _, _ = struct.unpack_from(
                '<HHHHHH',
                data,
                position + 24,
            )

            tag = strings[name_index]
            attributes: list[tuple[str, int | None, object, int]] = []
            attribute_position = position + 16 + attribute_start

            for index in range(attribute_count):
                offset = attribute_position + index * attribute_size
                _, attribute_name_index, raw_value_index = struct.unpack_from('<III', data, offset)
                _, _, value_type, typed_value = struct.unpack_from('<HBBI', data, offset + 12)
                attribute_name = strings[attribute_name_index]
                resource_id = (
                    resource_ids[attribute_name_index]
                    if attribute_name_index < len(resource_ids)
                    else None
                )
                rendered_value = (
                    strings[raw_value_index]
                    if raw_value_index != NO_INDEX
                    else typed_value
                )
                attributes.append(
                    (
                        attribute_name,
                        resource_id,
                        rendered_value,
                        value_type,
                    )
                )

            elements.append((tag, attributes))

        position += chunk_size

    return elements


def attributes_by_name(attributes: list[tuple[str, int | None, object, int]]) -> dict[str, tuple[int | None, object, int]]:
    return {
        name: (resource_id, value, value_type)
        for name, resource_id, value, value_type in attributes
    }


def main() -> None:
    path = Path(sys.argv[1])
    elements = parse_manifest(path)

    services = [attributes for tag, attributes in elements if tag == 'service']
    if not services:
        raise SystemExit('FAIL: service tag missing')

    service = services[0]
    service_attributes = attributes_by_name(service)
    if service_attributes.get('process', (None, None, None))[1] != ':trust':
        raise SystemExit(f"FAIL: service process={service_attributes.get('process')}")
    if service_attributes.get('foregroundServiceType', (None, None, None))[1] != 0x40000000:
        raise SystemExit('FAIL: specialUse FGS type missing')

    resource_order = [
        resource_id
        for _, resource_id, _, _ in service
        if resource_id is not None
    ]
    if resource_order != sorted(resource_order):
        rendered = [hex(value) for value in resource_order]
        raise SystemExit(f'FAIL: service attrs not sorted by resource id: {rendered}')

    data_elements = [attributes_by_name(attributes) for tag, attributes in elements if tag == 'data']
    if not any(
        attributes.get('scheme', (None, None, None))[1] == 'zorintrust'
        and attributes.get('host', (None, None, None))[1] == 'connect'
        for attributes in data_elements
    ):
        raise SystemExit('FAIL: zorintrust://connect deep-link data element missing')

    action_names = {
        attributes_by_name(attributes).get('name', (None, None, None))[1]
        for tag, attributes in elements
        if tag == 'action'
    }
    category_names = {
        attributes_by_name(attributes).get('name', (None, None, None))[1]
        for tag, attributes in elements
        if tag == 'category'
    }

    required_actions = {
        'android.intent.action.MAIN',
        'android.intent.action.VIEW',
    }
    required_categories = {
        'android.intent.category.LAUNCHER',
        'android.intent.category.DEFAULT',
        'android.intent.category.BROWSABLE',
    }

    if not required_actions.issubset(action_names):
        raise SystemExit('FAIL: VIEW deep-link action missing')
    if not required_categories.issubset(category_names):
        raise SystemExit('FAIL: BROWSABLE deep-link categories missing')

    print('manifest verify: OK process=:trust, FGS=specialUse, deep-link=zorintrust://connect, attrs sorted')


if __name__ == '__main__':
    main()

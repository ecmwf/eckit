#!/usr/bin/env python3
# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

"""Write golden.ekbf, the reference file for test_blockfile_golden.cc.

This is an independent implementation of the layout documented in
src/eckit/blockfile/Format.h (it does not use eckit), so the golden test checks
the C++ code against the specification. Blocks are compressed with the `zstd`
command-line tool. Re-run only on purpose, when the format version changes:

    python3 make_golden_blockfile.py golden.ekbf
"""

import struct
import subprocess
import sys

HEADER_MAGIC = 0x184D2A5A
TABLE_MAGIC = 0x184D2A5C
STRING, INTEGER, DOUBLE = 0, 1, 2

KEYS = [("class", STRING), ("param", INTEGER), ("level", DOUBLE)]
BLOCKS = [
    (["od", 130, 500.0], ['{"a":1}', '{"a":2}']),
    (["rd", 167, -1.5], ['{"b":"x"}']),
]


def compress(data):
    return subprocess.run(["zstd", "-q", "-c", "-9"], input=data, stdout=subprocess.PIPE, check=True).stdout


def header_frame(blocks, table_offset, records):
    payload = b"EKBF" + struct.pack("<IQQQ", 1, blocks, table_offset, records)
    payload += b"\0" * (64 - len(payload))
    return struct.pack("<II", HEADER_MAGIC, len(payload)) + payload


def main(path):
    out = bytearray(header_frame(0, 0, 0))
    entries = []
    strings = bytearray()
    string_offsets = {}

    for labels, records in BLOCKS:
        raw = "".join(r + "\n" for r in records).encode()
        frame = compress(raw)
        entry = struct.pack("<QQQI", len(out), len(frame), len(raw), len(records))
        for (name, kind), value in zip(KEYS, labels):
            if kind == STRING:
                data = value.encode()
                if value not in string_offsets:
                    string_offsets[value] = len(strings)
                    strings += data
                entry += struct.pack("<II", string_offsets[value], len(data))
            elif kind == INTEGER:
                entry += struct.pack("<q", value)
            else:
                entry += struct.pack("<d", value)
        entries.append(entry)
        out += frame

    table_offset = len(out)
    payload = struct.pack("<I", len(KEYS))
    for name, kind in KEYS:
        payload += struct.pack("<BBH", kind, 0, len(name)) + name.encode()
    payload += struct.pack("<IIQ", 28 + 8 * len(KEYS), 0, len(strings))
    payload += b"".join(entries) + bytes(strings)
    out += struct.pack("<II", TABLE_MAGIC, len(payload)) + payload

    total = sum(len(records) for _, records in BLOCKS)
    out[0 : len(header_frame(0, 0, 0))] = header_frame(len(BLOCKS), table_offset, total)

    with open(path, "wb") as f:
        f.write(out)


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "golden.ekbf")

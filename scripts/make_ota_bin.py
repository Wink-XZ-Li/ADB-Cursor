#!/usr/bin/env python3
"""Build Tuya MCU OTA payload: OADB header + RUN image from App HEX."""

from __future__ import annotations

import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from intelhex16 import load_hex

RUN_BASE = 0x1000
PAYLOAD_MAX = 0x77F0
OADB = b"OADB"


def crc16_ccitt(data: bytes) -> int:
    c = 0xFFFF
    for b in data:
        c ^= (b << 8)
        for _ in range(8):
            if c & 0x8000:
                c = ((c << 1) ^ 0x1021) & 0xFFFF
            else:
                c = (c << 1) & 0xFFFF
    return c


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("app_hex")
    p.add_argument("-o", "--output", required=True)
    args = p.parse_args()

    img, used = load_hex(args.app_hex)
    last = -1
    for a in range(RUN_BASE, RUN_BASE + PAYLOAD_MAX):
        if used[a]:
            last = a
    if last < 0:
        raise SystemExit("no RUN bytes in App HEX")
    payload_len = last - RUN_BASE + 1
    if payload_len > PAYLOAD_MAX:
        raise SystemExit("payload %u exceeds %u" % (payload_len, PAYLOAD_MAX))
    payload = bytes(img[RUN_BASE : RUN_BASE + payload_len])
    crc = crc16_ccitt(payload)
    hdr = bytearray(16)
    hdr[0:4] = OADB
    hdr[4] = payload_len & 0xFF
    hdr[5] = (payload_len >> 8) & 0xFF
    hdr[6] = 0
    hdr[7] = 0
    hdr[8] = crc & 0xFF
    hdr[9] = (crc >> 8) & 0xFF
    blob = bytes(hdr) + payload
    with open(args.output, "wb") as f:
        f.write(blob)
    print("OADB len=%u crc=0x%04X file=%u -> %s" % (payload_len, crc, len(blob), args.output))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

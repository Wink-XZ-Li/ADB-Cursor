#!/usr/bin/env python3
"""Merge Boot HEX with App HEX. App bytes below 0x1000 are dropped."""

from __future__ import annotations

import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from intelhex16 import load_hex, save_hex

BOOT_END = 0x1000
MAP_END = 0x10000


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("boot_hex")
    p.add_argument("app_hex")
    p.add_argument("-o", "--output", required=True)
    args = p.parse_args()

    boot_img, boot_used = load_hex(args.boot_hex)
    app_img, app_used = load_hex(args.app_hex)

    img = bytearray([0xFF] * MAP_END)
    used = bytearray(MAP_END)

    boot_max = -1
    for a in range(BOOT_END):
        if boot_used[a]:
            img[a] = boot_img[a]
            used[a] = 1
            boot_max = a
    if boot_max < 0 or boot_max >= BOOT_END:
        raise SystemExit("Boot HEX empty or exceeds 0x1000")

    dropped = 0
    app_min = MAP_END
    app_max = -1
    for a in range(MAP_END):
        if not app_used[a]:
            continue
        if a < BOOT_END:
            dropped += 1
            continue
        img[a] = app_img[a]
        used[a] = 1
        if a < app_min:
            app_min = a
        if a > app_max:
            app_max = a

    if dropped != 0:
        raise SystemExit("App HEX has %u bytes below 0x1000 (CONST/CODE in Boot region)" % dropped)
    if app_max < 0:
        raise SystemExit("App HEX has no bytes at/after 0x1000")
    if app_max > 0x87EF:
        raise SystemExit("App HEX last byte 0x%04X exceeds 0x87EF" % app_max)

    save_hex(args.output, img, used, 0, app_max)
    print("factory boot=0x0000-0x%04X app=0x%04X-0x%04X dropped_lt_1000=%u -> %s" % (
        boot_max, app_min, app_max, dropped, args.output))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

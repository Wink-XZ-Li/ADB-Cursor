#!/usr/bin/env python3
"""Load/save Intel HEX (16-bit code space)."""

from __future__ import annotations


def load_hex(path: str):
    img = bytearray([0xFF] * 0x10000)
    used = bytearray(0x10000)
    base = 0
    with open(path, "r", encoding="ascii") as f:
        for raw in f:
            line = raw.strip()
            if not line or not line.startswith(":"):
                continue
            body = bytes.fromhex(line[1:])
            n = body[0]
            addr = (body[1] << 8) | body[2]
            rtype = body[3]
            data = body[4:4 + n]
            if (sum(body[:5 + n]) & 0xFF) != 0:
                raise ValueError("bad checksum in %s: %s" % (path, line))
            if rtype == 0x00:
                a = (base + addr) & 0xFFFF
                for i, b in enumerate(data):
                    aa = (a + i) & 0xFFFF
                    img[aa] = b
                    used[aa] = 1
            elif rtype == 0x01:
                break
            elif rtype == 0x02:
                base = ((data[0] << 8) | data[1]) << 4
            elif rtype == 0x04:
                base = ((data[0] << 8) | data[1]) << 16
            elif rtype in (0x03, 0x05):
                continue
            else:
                raise ValueError("unsupported HEX type %u in %s" % (rtype, path))
    return img, used


def save_hex(path: str, img: bytearray, used: bytearray, lo: int, hi: int) -> None:
    lines = []

    def rec(addr, payload):
        n = len(payload)
        buf = bytes([n, (addr >> 8) & 0xFF, addr & 0xFF, 0x00]) + payload
        csum = (-sum(buf)) & 0xFF
        lines.append(":" + buf.hex().upper() + "%02X" % csum)

    addr = lo
    while addr <= hi:
        if used[addr] == 0:
            addr += 1
            continue
        start = addr
        chunk = bytearray()
        while addr <= hi and used[addr] and len(chunk) < 16:
            chunk.append(img[addr])
            addr += 1
        rec(start, bytes(chunk))
    lines.append(":00000001FF")
    with open(path, "w", encoding="ascii", newline="\n") as f:
        f.write("\n".join(lines) + "\n")

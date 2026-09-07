"""TM1640 RAM map from docs/led_segment_map.md.

Index n is written at command 0xC0+n (datasheet GRID n+1).
Tens/ones are dual-COM. Icon bits: silk SEG1=A=bit7 ... SEG7=G=bit1.
"""

GRID_COUNT = 16

TENS = (8, 9)
ONES = (7, 6)
G_WIFI = 10
G_HIGH = 15
G_COOL = 0
G_FAN = 1

# Silk SEG7..SEG4 on SLED2/3, using A=bit7 ... G=bit1
SEG7 = 0x02
SEG6 = 0x04
SEG5 = 0x08
SEG4 = 0x10

ICON_WIFI = (G_WIFI, SEG7)
ICON_COOL = (G_COOL, SEG6)
ICON_DRY = (G_COOL, SEG4)
ICON_FAN = (G_FAN, SEG7)
ICON_HEAT = (G_FAN, SEG5)
ICON_LOW = (G_WIFI, SEG6)
ICON_MED = (G_WIFI, SEG4)
ICON_HIGH = (G_HIGH, SEG7)
ICON_TURBO = (G_HIGH, SEG5)

# A=bit7 ... G=bit1, bit0 unused
N = (
    0xFC,  # 0 ABCDEF
    0x60,  # 1 BC
    0xDA,  # 2 ABDEG
    0xF2,  # 3 ABCDG
    0x66,  # 4 BCFG
    0xB6,  # 5 ACDFG
    0xBE,  # 6 ACDEFG
    0xE0,  # 7 ABC
    0xFE,  # 8 ABCDEFG
    0xF6,  # 9 ABCDFG
)


def blank():
    return [0] * GRID_COUNT


def icon(grid, mask):
    buf = blank()
    buf[grid] = mask
    return buf


def tens_digit(n):
    buf = blank()
    buf[TENS[0]] = buf[TENS[1]] = N[n]
    return buf


def ones_digit(n):
    buf = blank()
    buf[ONES[0]] = buf[ONES[1]] = N[n]
    return buf

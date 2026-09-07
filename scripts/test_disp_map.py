"""Spec tests: one LED / one digit at a time, per led_segment_map.md."""
from disp_map import (
    G_COOL,
    G_FAN,
    G_HIGH,
    G_WIFI,
    ICON_COOL,
    ICON_DRY,
    ICON_FAN,
    ICON_HEAT,
    ICON_HIGH,
    ICON_LOW,
    ICON_MED,
    ICON_TURBO,
    ICON_WIFI,
    N,
    icon,
    ones_digit,
    tens_digit,
)


def _ones(buf):
    return [i for i, v in enumerate(buf) if v]


def test_each_icon_lights_exactly_one_grid_and_one_mask():
    cases = {
        "wifi": ICON_WIFI,
        "cool": ICON_COOL,
        "dry": ICON_DRY,
        "fan": ICON_FAN,
        "heat": ICON_HEAT,
        "low": ICON_LOW,
        "med": ICON_MED,
        "high": ICON_HIGH,
        "turbo": ICON_TURBO,
    }
    seen = []
    for name, (grid, mask) in cases.items():
        buf = icon(grid, mask)
        assert _ones(buf) == [grid], name
        assert buf[grid] == mask, name
        assert bin(mask).count("1") == 1, name
        seen.append((grid, mask))
    assert len(set(seen)) == len(seen)


def test_icon_grids_match_section1():
    assert ICON_WIFI[0] == ICON_LOW[0] == ICON_MED[0] == G_WIFI == 10
    assert ICON_HIGH[0] == ICON_TURBO[0] == G_HIGH == 15
    assert ICON_COOL[0] == ICON_DRY[0] == G_COOL == 0
    assert ICON_FAN[0] == ICON_HEAT[0] == G_FAN == 1


def test_digit_8_is_all_segments_except_bit0():
    assert N[8] == 0xFE
    t = tens_digit(8)
    assert t[8] == t[9] == 0xFE
    assert t[7] == t[6] == 0
    o = ones_digit(8)
    assert o[7] == o[6] == 0xFE
    assert o[8] == o[9] == 0


def test_digit_1_uses_bc():
    assert N[1] == 0x60
    t = tens_digit(1)
    assert _ones(t) == [8, 9]


if __name__ == "__main__":
    test_each_icon_lights_exactly_one_grid_and_one_mask()
    test_icon_grids_match_section1()
    test_digit_8_is_all_segments_except_bit0()
    test_digit_1_uses_bc()
    print("disp_map tests ok")

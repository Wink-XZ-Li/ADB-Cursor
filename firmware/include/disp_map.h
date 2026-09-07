#ifndef DISP_MAP_H
#define DISP_MAP_H

/*
 * TM1640 display-RAM index n is command 0xC0+n (datasheet GRID n+1).
 * From docs/led_segment_map.md: dual-COM digits, A=bit7 ... G=bit1.
 * Icon silk SEG1=A ... SEG7=G on the same bit order.
 */

#define DISP_TM_TENS_0   8
#define DISP_TM_TENS_1   9
#define DISP_TM_ONES_0   7
#define DISP_TM_ONES_1   6
#define DISP_TM_WIFI     10
#define DISP_TM_HIGH     15
#define DISP_TM_COOL     0
#define DISP_TM_FAN      1

#define DISP_SEG7        0x02
#define DISP_SEG6        0x04
#define DISP_SEG5        0x08
#define DISP_SEG4        0x10

#define DISP_ICON_WIFI   DISP_SEG7
#define DISP_ICON_LOW    DISP_SEG6
#define DISP_ICON_MED    DISP_SEG4
#define DISP_ICON_HIGH   DISP_SEG7
#define DISP_ICON_TURBO  DISP_SEG5
#define DISP_ICON_COOL   DISP_SEG6
#define DISP_ICON_DRY    DISP_SEG4
#define DISP_ICON_FAN    DISP_SEG7
#define DISP_ICON_HEAT   DISP_SEG5

#endif

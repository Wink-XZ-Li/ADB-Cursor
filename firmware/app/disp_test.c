#include "board.h"
#include "tm1640.h"
#include "log_uart.h"
#include "disp_test.h"
#include "disp_map.h"

static unsigned char s_step;

/* 0-9, A=bit7 ... G=bit1 */
static unsigned char code s_n[10] = {
    0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6
};

static void fill0(unsigned char *buf)
{
    unsigned char i;
    for (i = 0; i < TM1640_GRID_COUNT; i++)
    {
        buf[i] = 0;
    }
}

static void light_pair(unsigned char *buf, unsigned char a, unsigned char b, unsigned char v)
{
    buf[a] = v;
    buf[b] = v;
}

void disp_test_init(void)
{
    s_step = 0;
    tm1640_init();
}

void disp_test_step(unsigned int tick)
{
    unsigned char buf[16];
    unsigned char idx;
    unsigned char n;

    if ((tick & 0x01) != 0)
    {
        return;
    }

    fill0(buf);
    idx = s_step;

    if (idx == 0)
    {
        buf[DISP_TM_WIFI] = DISP_ICON_WIFI;
        log_puts("DISP wifi\r\n");
    }
    else if (idx == 1)
    {
        buf[DISP_TM_COOL] = DISP_ICON_COOL;
        log_puts("DISP cool\r\n");
    }
    else if (idx == 2)
    {
        buf[DISP_TM_COOL] = DISP_ICON_DRY;
        log_puts("DISP dry\r\n");
    }
    else if (idx == 3)
    {
        buf[DISP_TM_FAN] = DISP_ICON_FAN;
        log_puts("DISP fan\r\n");
    }
    else if (idx == 4)
    {
        buf[DISP_TM_FAN] = DISP_ICON_HEAT;
        log_puts("DISP heat\r\n");
    }
    else if (idx == 5)
    {
        buf[DISP_TM_WIFI] = DISP_ICON_LOW;
        log_puts("DISP low\r\n");
    }
    else if (idx == 6)
    {
        buf[DISP_TM_WIFI] = DISP_ICON_MED;
        log_puts("DISP med\r\n");
    }
    else if (idx == 7)
    {
        buf[DISP_TM_HIGH] = DISP_ICON_HIGH;
        log_puts("DISP high\r\n");
    }
    else if (idx == 8)
    {
        buf[DISP_TM_HIGH] = DISP_ICON_TURBO;
        log_puts("DISP turbo\r\n");
    }
    else if (idx < 19)
    {
        n = (unsigned char)(idx - 9);
        light_pair(buf, DISP_TM_TENS_0, DISP_TM_TENS_1, s_n[n]);
        log_puts("DISP tens=");
        log_u16((unsigned int)n);
        log_puts("\r\n");
    }
    else
    {
        n = (unsigned char)(idx - 19);
        light_pair(buf, DISP_TM_ONES_0, DISP_TM_ONES_1, s_n[n]);
        log_puts("DISP ones=");
        log_u16((unsigned int)n);
        log_puts("\r\n");
    }

    tm1640_display(buf);
    s_step++;
    if (s_step >= 29)
    {
        s_step = 0;
    }
}

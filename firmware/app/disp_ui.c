#include "board.h"
#include "tm1640.h"
#include "disp_ui.h"
#include "disp_map.h"

static unsigned char code s_n[10] = {
    0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6
};
static unsigned char code s_e = 0x9E;
static unsigned char code s_dash = 0x02;

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

void disp_ui_init(void)
{
    tm1640_init();
}

void disp_ui_draw(
    unsigned char power,
    unsigned char saver,
    unsigned char mode,
    unsigned char fan,
    unsigned char show_num,
    unsigned char num,
    unsigned char timer_lamp,
    unsigned char wifi_lamp,
    unsigned char dim,
    unsigned char overlay)
{
    unsigned char buf[16];
    unsigned char t;
    unsigned char o;
    unsigned char br;

    fill0(buf);

    if (overlay == DISP_OV_DASH)
    {
        light_pair(buf, DISP_TM_TENS_0, DISP_TM_TENS_1, s_dash);
        light_pair(buf, DISP_TM_ONES_0, DISP_TM_ONES_1, s_dash);
    }
    else if (overlay == DISP_OV_E1)
    {
        light_pair(buf, DISP_TM_TENS_0, DISP_TM_TENS_1, s_e);
        light_pair(buf, DISP_TM_ONES_0, DISP_TM_ONES_1, s_n[1]);
    }
    else if (overlay == DISP_OV_E2)
    {
        light_pair(buf, DISP_TM_TENS_0, DISP_TM_TENS_1, s_e);
        light_pair(buf, DISP_TM_ONES_0, DISP_TM_ONES_1, s_n[2]);
    }
    else if ((show_num != 0) && (num < 100U))
    {
        t = (unsigned char)(num / 10U);
        o = (unsigned char)(num % 10U);
        light_pair(buf, DISP_TM_TENS_0, DISP_TM_TENS_1, s_n[t]);
        light_pair(buf, DISP_TM_ONES_0, DISP_TM_ONES_1, s_n[o]);
    }

    if ((power != 0) && (saver == 0))
    {
        if (mode == 0)
        {
            buf[DISP_TM_COOL] |= DISP_ICON_COOL;
        }
        else if (mode == 1)
        {
            buf[DISP_TM_COOL] |= DISP_ICON_DRY;
        }
        else if (mode == 2)
        {
            buf[DISP_TM_FAN] |= DISP_ICON_HEAT;
        }
        else
        {
            buf[DISP_TM_FAN] |= DISP_ICON_FAN;
        }

        if (fan == 0)
        {
            buf[DISP_TM_WIFI] |= DISP_ICON_LOW;
        }
        else if (fan == 1)
        {
            buf[DISP_TM_WIFI] |= DISP_ICON_MED;
        }
        else if (fan == 2)
        {
            buf[DISP_TM_HIGH] |= DISP_ICON_HIGH;
        }
        else
        {
            buf[DISP_TM_HIGH] |= DISP_ICON_TURBO;
        }
    }

    if (timer_lamp != 0)
    {
        buf[DISP_TM_FAN] |= DISP_ICON_TIMER;
    }
    if (wifi_lamp != 0)
    {
        buf[DISP_TM_WIFI] |= DISP_ICON_WIFI;
    }

    if (dim != 0)
    {
        br = TM1640_BR_DIM;
    }
    else
    {
        br = TM1640_BR_FULL;
    }
    tm1640_display_br(buf, br);
}

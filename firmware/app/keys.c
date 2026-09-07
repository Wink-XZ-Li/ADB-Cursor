#include "board.h"
#include "keys.h"
#include "TKDriver.h"

/* docs/key_map.md: power/fan/timer/mode/up/down */
static unsigned char code s_tk[KEY_N] = {28, 23, 18, 10, 21, 8};

static unsigned long xdata s_now;
static unsigned int xdata s_hold[KEY_N];
static unsigned char xdata s_long[KEY_N];
static unsigned char xdata s_evt[KEY_N];

static void keys_tk_gpio_init(void)
{
    P0CON |= 0x10;
    P0PH &= 0xEF;
    P04 = 1;

    P1CON |= 0x05;
    P1PH &= 0xFA;
    P10 = 1;
    P12 = 1;

    P2CON |= 0xA4;
    P2PH &= 0x5B;
    P22 = 1;
    P25 = 1;
    P27 = 1;
}

void keys_init(void)
{
    unsigned char i;

    keys_tk_gpio_init();
    IE1 |= 0x10;
    EA = 1;
    board_wdt_feed();
    TouchKeyInit();
    board_wdt_feed();
    s_now = 0;
    for (i = 0; i < KEY_N; i++)
    {
        s_hold[i] = 0;
        s_long[i] = 0;
        s_evt[i] = KEY_EVT_NONE;
    }
}

unsigned char keys_take_evt(unsigned char id)
{
    unsigned char e;

    e = s_evt[id];
    s_evt[id] = KEY_EVT_NONE;
    return e;
}

void keys_poll(void)
{
    unsigned char i;
    unsigned char down;

    if ((SOCAPI_TouchKeyStatus & 0x80) != 0)
    {
        SOCAPI_TouchKeyStatus &= 0x7F;
        s_now = TouchKeyScan();
        TouchKeyRestart();
    }

    for (i = 0; i < KEY_N; i++)
    {
        down = (unsigned char)((s_now >> s_tk[i]) & 1UL);
        if (down != 0)
        {
            if (s_hold[i] < 60000U)
            {
                s_hold[i]++;
            }
            if (s_hold[i] == 1U)
            {
                if ((i == KEY_MODE) || (i == KEY_TIMER) || (i == KEY_UP) || (i == KEY_DOWN))
                {
                    s_evt[i] = KEY_EVT_CLICK;
                }
                if ((i == KEY_FAN) && (s_evt[i] == KEY_EVT_NONE))
                {
                    /* on-unit fan click is decided in HMI using power state;
                     * send CLICK immediately; HMI ignores it when off. */
                    s_evt[i] = KEY_EVT_CLICK;
                }
            }
            if ((s_hold[i] == 3000U) && (s_long[i] == 0))
            {
                s_long[i] = 1;
                if ((i == KEY_POWER) || (i == KEY_FAN))
                {
                    s_evt[i] = KEY_EVT_LONG;
                }
            }
            if (((i == KEY_UP) || (i == KEY_DOWN)) &&
                (s_hold[i] >= 400U) &&
                (((s_hold[i] - 400U) % 200U) == 0))
            {
                s_evt[i] = KEY_EVT_REPEAT;
            }
        }
        else
        {
            if ((i == KEY_POWER) && (s_hold[i] >= 40U))
            {
                s_evt[i] = KEY_EVT_CLICK;
            }
            s_hold[i] = 0;
            s_long[i] = 0;
        }
    }
}

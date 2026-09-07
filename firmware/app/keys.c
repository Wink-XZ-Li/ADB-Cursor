#include "board.h"
#include "keys.h"
#include "log_uart.h"
#include "disp_test.h"
#include "TKDriver.h"

extern unsigned char data CurrentChannelMax;

static unsigned long s_prev;
static unsigned int s_done;

/*
 * Official SC95F TouchKey guide (T1): set each TK pin push-pull high
 * before TouchKeyInit. Do not drive CMOD (P0.7).
 * TK8=P1.0, TK10=P1.2, TK18=P2.2, TK21=P2.5, TK23=P2.7, TK28=P0.4.
 */
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
    keys_tk_gpio_init();
    IE1 |= 0x10;
    EA = 1;
    board_wdt_feed();
    TouchKeyInit();
    board_wdt_feed();
    s_prev = 0;
    s_done = 0;
    disp_show_u8(CurrentChannelMax);
}

void keys_log_status(void)
{
    log_puts("TK st=");
    log_u16((unsigned int)SOCAPI_TouchKeyStatus);
    log_puts(" n=");
    log_u16((unsigned int)CurrentChannelMax);
    log_puts(" done=");
    log_u16(s_done);
    log_puts(" ie1=");
    log_u16((unsigned int)IE1);
    log_puts("\r\n");
}

void keys_poll(void)
{
    unsigned long now;
    unsigned long rose;
    unsigned char ch;

    if ((SOCAPI_TouchKeyStatus & 0x80) == 0)
    {
        return;
    }
    SOCAPI_TouchKeyStatus &= 0x7F;

    now = TouchKeyScan();
    TouchKeyRestart();
    s_done++;

    rose = now & ~s_prev;
    s_prev = now;
    if (rose == 0)
    {
        return;
    }

    for (ch = 0; ch < 32; ch++)
    {
        if (((rose >> ch) & 1UL) != 0)
        {
            log_puts("KEY tk=");
            log_u16((unsigned int)ch);
            log_puts("\r\n");
            disp_show_u8(ch);
        }
    }
}

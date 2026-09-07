#include "board.h"
#include "timebase.h"
#include "pwr_uart.h"

/*
 * Timer0 mode 1, clock = Fsys (TMCON.0).
 * Official datasheet + C51 demo: overflow time = count / Fsys.
 * 32000 counts @ 32 MHz = 1 ms. Manual reload; poll TF0, no ISR.
 */

#define T0_RELOAD_1MS  ((unsigned int)(65536U - 32000U))

void timebase_init(void)
{
    TMCON |= 0x01;
    TMOD = (TMOD & 0xF0) | 0x01;
    TR0 = 0;
    ET0 = 0;
    TF0 = 0;
}

void delay_ms(unsigned int ms)
{
    while (ms != 0)
    {
        TL0 = (unsigned char)(T0_RELOAD_1MS & 0xFF);
        TH0 = (unsigned char)(T0_RELOAD_1MS >> 8);
        TF0 = 0;
        TR0 = 1;
        while (TF0 == 0)
        {
            pwr_uart_poll_rx();
            board_wdt_feed();
        }
        TR0 = 0;
        TF0 = 0;
        ms--;
    }
}

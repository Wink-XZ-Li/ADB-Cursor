#include "board.h"
#include "log_uart.h"
#include "timebase.h"
#include "disp_test.h"
#include "beep.h"
#include "keys.h"

void board_wdt_feed(void)
{
    WDTCON |= 0x10;
}

void board_init(void)
{
    SC95F8763_NIO_Init();

    timebase_init();
    log_uart_init();
    disp_test_init();
    beep_init();
    beep_power_on();
    keys_init();
    EA = 1;
    board_wdt_feed();
}

void main(void)
{
    unsigned int tick;
    unsigned int ms;

    tick = 0;
    ms = 0;
    board_init();
    log_puts("\r\n");
    log_banner(0);

    while (1)
    {
        delay_ms(1);
        keys_poll();
        ms++;
        if (ms >= 500)
        {
            ms = 0;
            tick++;
            log_banner(tick);
            keys_log_status();
        }
    }
}

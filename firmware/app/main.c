#include "board.h"
#include "log_uart.h"
#include "timebase.h"
#include "disp_ui.h"
#include "beep.h"
#include "keys.h"
#include "hmi.h"

void board_wdt_feed(void)
{
    WDTCON |= 0x10;
}

void board_init(void)
{
    SC95F8763_NIO_Init();

    timebase_init();
    log_uart_init();
    disp_ui_init();
    beep_init();
    beep_power_on();
    keys_init();
    hmi_init();
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
    hmi_log_status();

    while (1)
    {
        delay_ms(1);
        keys_poll();
        hmi_poll();
        ms++;
        if (ms >= 500)
        {
            ms = 0;
            tick++;
            log_banner(tick);
            hmi_log_status();
        }
    }
}

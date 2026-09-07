#include "board.h"
#include "log_uart.h"
#include "timebase.h"
#include "disp_ui.h"
#include "beep.h"
#include "keys.h"
#include "hmi.h"
#include "pwr_link.h"

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
    pwr_link_init();
    EA = 1;
    board_wdt_feed();
}

void main(void)
{
    board_init();
    log_puts("\r\n");
    log_banner(0);
    hmi_log_status();

    while (1)
    {
        delay_ms(1);
        keys_poll();
        hmi_poll();
        pwr_link_poll();
    }
}

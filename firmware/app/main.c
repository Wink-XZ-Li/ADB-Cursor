#include "board.h"
#include "log_uart.h"
#include "timebase.h"
#include "disp_test.h"

void board_wdt_feed(void)
{
    WDTCON |= 0x10;
}

void board_heartbeat_toggle(void)
{
    HEARTBEAT_PIN = (bit)(HEARTBEAT_PIN == 0);
}

void board_init(void)
{
    SC95F8763_NIO_Init();

    P0CON |= 0x10;
    P0PH  &= 0xEF;
    HEARTBEAT_PIN = 0;

    timebase_init();
    log_uart_init();
    disp_test_init();
    board_wdt_feed();
}

void main(void)
{
    unsigned int tick;

    tick = 0;
    board_init();
    log_puts("\r\n");
    log_banner(0);

    while (1)
    {
        tick++;
        board_heartbeat_toggle();
        log_banner(tick);
        disp_test_step(tick);
        delay_ms(500);
        board_wdt_feed();
    }
}

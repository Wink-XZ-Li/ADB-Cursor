#include "board.h"
#include "log_uart.h"

void board_wdt_feed(void)
{
    WDTCON |= 0x10;
}

void board_heartbeat_toggle(void)
{
    HEARTBEAT_PIN = (bit)(HEARTBEAT_PIN == 0);
}

void delay_ms_approx(unsigned int ms)
{
    unsigned int i;
    unsigned int j;

    /* Rough busy-wait at 32 MHz / C51 small model. Not a calibrated timer. */
    for (i = 0; i < ms; i++)
    {
        board_wdt_feed();
        for (j = 0; j < 400; j++)
        {
            _nop_();
        }
    }
}

void board_init(void)
{
    SC95F8763_NIO_Init();

    P0CON |= 0x10;
    P0PH  &= 0xEF;
    HEARTBEAT_PIN = 0;

    log_uart_init();
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
        delay_ms_approx(500);
        board_wdt_feed();
    }
}

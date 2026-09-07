#include "board.h"
#include "log_uart.h"

/*
 * USCI0 UART comes from:
 * - Official header SC95F876x_C.H (US0CONx, OTCON)
 * - Official datasheet SC95F876x v1.0cn / v0.1en: USMD0=11 UART, Mode 1,
 *   BaudRate = fsys / BAUD1, TI/RI write-1-to-clear
 * - Official C51 demo USCI0 Uart0_Init pin/mode sequence (P0.5/P0.6 pull-up
 *   input, OTCON|=0x30, US0CON0=0x50)
 * TX wait uses timeout; do not dead-spin if the peripheral never sets TI.
 */

static unsigned int s_tx_timeout;

static void log_clear_ti(void)
{
    US0CON0 &= 0xFE;
}

void log_uart_init(void)
{
    P0CON &= 0x9F;
    P0PH  |= 0x60;

    OTCON |= 0x30;
    US0CON0 = 0x50;
    US0CON1 = (unsigned char)(LOG_BAUD_DIV & 0xFF);
    US0CON2 = (unsigned char)((LOG_BAUD_DIV >> 8) & 0xFF);
}

void log_putc(unsigned char c)
{
    s_tx_timeout = 0x8000;
    US0CON3 = c;
    while (((US0CON0 & 0x02) == 0) && (s_tx_timeout != 0))
    {
        s_tx_timeout--;
        board_wdt_feed();
    }
    log_clear_ti();
}

void log_puts(char code *s)
{
    while (*s != 0)
    {
        log_putc((unsigned char)(*s));
        s++;
    }
}

void log_u16(unsigned int v)
{
    unsigned char buf[5];
    unsigned char n;
    unsigned char i;

    if (v == 0)
    {
        log_putc('0');
        return;
    }

    n = 0;
    while (v != 0)
    {
        buf[n++] = (unsigned char)(v % 10);
        v = v / 10;
    }
    i = n;
    while (i != 0)
    {
        i--;
        log_putc((unsigned char)('0' + buf[i]));
    }
}

void log_hex8(unsigned char v)
{
    unsigned char n;

    n = (unsigned char)(v >> 4);
    if (n < 10U)
    {
        log_putc((unsigned char)('0' + n));
    }
    else
    {
        log_putc((unsigned char)('A' + n - 10U));
    }
    n = (unsigned char)(v & 0x0FU);
    if (n < 10U)
    {
        log_putc((unsigned char)('0' + n));
    }
    else
    {
        log_putc((unsigned char)('A' + n - 10U));
    }
}

void log_banner(unsigned int tick)
{
    log_puts(FW_BANNER);
    log_puts(" ");
    log_puts(FW_BUILD_DATE);
    log_puts(" tick=");
    log_u16(tick);
    log_puts("\r\n");
}

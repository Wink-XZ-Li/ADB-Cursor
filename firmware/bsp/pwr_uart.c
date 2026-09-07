#include "board.h"
#include "pwr_uart.h"

/*
 * USCI2 UART from official SC95F876x demo USCIX_Init.c (USCI2 pins/mode/baud)
 * and datasheet: P4.4=USTX2, P4.5=USRX2. Shared SFR window needs USXINX=2.
 * TI/RI clear matches the demo and working log_uart (USCI0).
 *
 * RX is polled, not interrupt 16. Enabling USCI2 IRQ also arms TI; after the
 * comm cable is pulled the TI/RI flags can stick and starve the 1 ms loop
 * that drives the 5 s lost timer. Polling in delay_ms() keeps 4800 bps
 * without that lockup. WiFi UART P2.0/P2.1 is not initialized here.
 */

#define PWR_RX_N  64

static unsigned int s_tx_timeout;
static unsigned char xdata s_rxq[PWR_RX_N];
static unsigned char xdata s_rxh;
static unsigned char xdata s_rxt;
static unsigned char s_ready;

static void pwr_select(void)
{
    USXINX = 2;
}

static void pwr_rx_push(unsigned char b)
{
    unsigned char n;

    n = (unsigned char)((s_rxh + 1U) & (unsigned char)(PWR_RX_N - 1));
    if (n != s_rxt)
    {
        s_rxq[s_rxh] = b;
        s_rxh = n;
    }
}

void pwr_uart_poll_rx(void)
{
    if (s_ready == 0)
    {
        return;
    }
    pwr_select();
    if ((USXCON0 & 0x01) != 0)
    {
        pwr_rx_push(USXCON3);
        USXCON0 &= 0xFD;
    }
}

void pwr_uart_init(void)
{
    s_rxh = 0;
    s_rxt = 0;

    pwr_select();
    TMCON |= 0xC0;
    P4CON &= ~0x30;
    P4PH |= 0x30;
    USXCON0 = 0x50;
    USXCON1 = (unsigned char)(PWR_BAUD_DIV & 0xFF);
    USXCON2 = (unsigned char)((PWR_BAUD_DIV >> 8) & 0xFF);
    IE2 &= 0xFD;
    s_ready = 1;
}

void pwr_uart_putc(unsigned char c)
{
    pwr_select();
    s_tx_timeout = 0x8000;
    USXCON3 = c;
    while (((USXCON0 & 0x02) == 0) && (s_tx_timeout != 0))
    {
        if ((USXCON0 & 0x01) != 0)
        {
            pwr_rx_push(USXCON3);
            USXCON0 &= 0xFD;
        }
        s_tx_timeout--;
        board_wdt_feed();
    }
    USXCON0 &= 0xFE;
}

unsigned char pwr_uart_getc(unsigned char *out)
{
    pwr_uart_poll_rx();
    if (s_rxt == s_rxh)
    {
        return 0;
    }
    *out = s_rxq[s_rxt];
    s_rxt = (unsigned char)((s_rxt + 1U) & (unsigned char)(PWR_RX_N - 1));
    return 1;
}

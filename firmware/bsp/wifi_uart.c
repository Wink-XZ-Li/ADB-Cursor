#include "board.h"
#include "wifi_uart.h"
#include "log_uart.h"

/*
 * WiFi module on P2.1 TX / P2.0 RX = UART0, 9600 8N1.
 * Official 8763 Uart_Init.c Timer2 baud (Timer1 is IR).
 * Datasheet: SMOD=0 → BaudRate = Fsys / RCAP, RCAP > 0x0010.
 * RX: interrupt 4 plus poll (same reason power UART dropped IRQ).
 */

#define RX_N           64U
#define T0_RELOAD_1MS  ((unsigned int)(65536U - 32000U))

static unsigned char xdata s_rx[RX_N];
static unsigned char data s_in;
static unsigned char data s_out;
static unsigned char data s_ready;

static void baud_arm(void)
{
    unsigned int div;

    TXINX = 0x02;
    TMCON |= 0x04;
    TXMOD = 0x00;
    TXCON = 0x30;
    div = (unsigned int)(SYSCLK_HZ / WIFI_UART_BAUD);
    RCAPXH = (unsigned char)(div / 256U);
    RCAPXL = (unsigned char)(div % 256U);
    TRX = 0;
    ET2 = 0;
}

void wifi_uart_power(void)
{
    /* Active-low: P2.6 low turns on module 3.3 V (P_Wifi_Power). */
    P2VO &= 0xBF;
    WIFI_PWR_PIN = 0;
    P2CON |= 0x40;
    P2PH  &= 0xBF;
    WIFI_PWR_PIN = 0;
}

void wifi_uart_pins(void)
{
    wifi_uart_power();
    P2VO &= 0xFC;
    P2CON &= 0xFC;
    P2PH  |= 0x03;
}

static void rx_push(unsigned char b)
{
    unsigned char n;

    n = (unsigned char)((s_in + 1U) & (unsigned char)(RX_N - 1U));
    if (n != s_out)
    {
        s_rx[s_in] = b;
        s_in = n;
    }
}

static void probe_edges(void)
{
    unsigned int e0;
    unsigned int e1;
    unsigned int ms;
    unsigned char l0;
    unsigned char l1;
    unsigned char p;

    e0 = 0;
    e1 = 0;
    l0 = P20;
    l1 = P21;
    for (ms = 0; ms < 80U; ms++)
    {
        TL0 = (unsigned char)(T0_RELOAD_1MS & 0xFF);
        TH0 = (unsigned char)(T0_RELOAD_1MS >> 8);
        TF0 = 0;
        TR0 = 1;
        while (TF0 == 0)
        {
            p = P20;
            if (p != l0)
            {
                e0++;
                l0 = p;
            }
            p = P21;
            if (p != l1)
            {
                e1++;
                l1 = p;
            }
            board_wdt_feed();
        }
        TR0 = 0;
        TF0 = 0;
    }
    log_puts("WIFI e0=");
    log_u16(e0);
    log_puts(" e1=");
    log_u16(e1);
    log_puts("\r\n");
}

void wifi_uart_init(void)
{
    s_ready = 0;
    s_in = 0;
    s_out = 0;
    wifi_uart_pins();
    probe_edges();
    SCON = 0x50;
    baud_arm();
    TI = 0;
    RI = 0;
    EUART = 1;
    s_ready = 1;
}

void wifi_uart_poll_rx(void)
{
    if (s_ready == 0)
    {
        return;
    }
    if (RI)
    {
        RI = 0;
        rx_push(SBUF);
    }
}

unsigned char wifi_uart_putc(unsigned char c)
{
    unsigned int t;

    baud_arm();
    EUART = 0;
    TI = 0;
    SBUF = c;
    t = 0xFFFF;
    while ((TI == 0) && (t != 0))
    {
        t--;
        board_wdt_feed();
    }
    if (TI != 0)
    {
        TI = 0;
    }
    EUART = 1;
    wifi_uart_poll_rx();
    return (unsigned char)((t != 0) ? 1 : 0);
}

unsigned char wifi_uart_rx_take(unsigned char *c)
{
    if (s_in == s_out)
    {
        return 0;
    }
    *c = s_rx[s_out];
    s_out = (unsigned char)((s_out + 1U) & (unsigned char)(RX_N - 1U));
    return 1;
}

void wifi_uart_isr(void) interrupt 4
{
    if (RI)
    {
        RI = 0;
        rx_push(SBUF);
    }
    if (TI)
    {
        TI = 0;
    }
}

#include "board.h"
#include "pwr_link.h"
#include "pwr_uart.h"
#include "hmi.h"
#include "ntc.h"
#include "log_uart.h"

/*
 * Indoor→outdoor 21-byte 66 99 and outdoor→indoor 33-byte 55 5A
 * from the authorized protocol extract (not old ADB source).
 * Send period 150 ms. Lost after 5 s without a valid 55 5A frame.
 * Byte3: indoor NTC °C+40 once ntc_ready(); until then Byte3 = setpoint
 * (logged cur=set). Open/short keep last valid.
 */

#define PWR_TX_N      21
#define PWR_RX_N      33
#define PWR_TX_MS     150U
#define PWR_LOST_MS   5000U

#define ST_55  0
#define ST_5A  1
#define ST_DAT 2

static unsigned char code s_mode_tx[4] = {3, 0, 2, 1};

static unsigned char xdata s_tx[PWR_TX_N];
static unsigned char xdata s_rx[PWR_RX_N];
static unsigned char xdata s_st;
static unsigned char xdata s_n;
static unsigned char xdata s_lost;
static unsigned char xdata s_fault;
static unsigned char xdata s_have_rx;
static unsigned char xdata s_last_tx4;
static unsigned char xdata s_last_tx5;
static unsigned char xdata s_last_tx2;
static unsigned char xdata s_last_tx3;
static unsigned char xdata s_last_ver;
static unsigned char xdata s_last_type;
static unsigned char xdata s_last_fault_log;
static unsigned char xdata s_rx_ver;
static unsigned char xdata s_rx_type;
static unsigned char xdata s_rx_fault;
static unsigned int xdata s_tx_ms;
static unsigned int xdata s_lost_ms;

static unsigned char checksum(unsigned char *buf, unsigned char n)
{
    unsigned char s;
    unsigned char i;

    s = 0;
    for (i = 0; i < n; i++)
    {
        s = (unsigned char)(s + buf[i]);
    }
    return s;
}

static unsigned char proto_fan(unsigned char power, unsigned char fan)
{
    if (power == 0)
    {
        return 0;
    }
    return (unsigned char)(fan + 1U);
}

static void log_tx_frame(void)
{
    unsigned char i;

    log_puts("PWR tx");
    for (i = 0; i < 6U; i++)
    {
        log_putc(' ');
        log_hex8(s_tx[i]);
    }
    log_puts(" sum=");
    log_hex8(s_tx[20]);
    if (ntc_ready() != 0)
    {
        log_puts(" cur=ntc\r\n");
    }
    else
    {
        log_puts(" cur=set\r\n");
    }
}

static void build_tx(void)
{
    unsigned char i;
    unsigned char power;
    unsigned char mode;
    unsigned char fan;
    unsigned char set_c;

    power = hmi_power();
    mode = hmi_mode();
    fan = hmi_fan();
    set_c = hmi_setpoint_c();
    if (mode > 3U)
    {
        mode = 0;
    }

    s_tx[0] = 0x66;
    s_tx[1] = 0x99;
    s_tx[2] = (unsigned char)(set_c + 40U);
    if (ntc_ready() != 0)
    {
        s_tx[3] = (unsigned char)(ntc_c() + 40U);
    }
    else
    {
        s_tx[3] = s_tx[2];
    }
    s_tx[4] = (unsigned char)((proto_fan(power, fan) << 4) | s_mode_tx[mode]);
    s_tx[5] = (unsigned char)(power & 0x01);
    for (i = 6; i < 20U; i++)
    {
        s_tx[i] = 0;
    }
    s_tx[20] = checksum(s_tx, 20);
}

static void send_tx(void)
{
    unsigned char i;
    unsigned char changed;

    build_tx();
    for (i = 0; i < PWR_TX_N; i++)
    {
        pwr_uart_putc(s_tx[i]);
    }

    changed = 0;
    if ((s_tx[2] != s_last_tx2) || (s_tx[3] != s_last_tx3)
        || (s_tx[4] != s_last_tx4) || (s_tx[5] != s_last_tx5))
    {
        changed = 1;
        s_last_tx2 = s_tx[2];
        s_last_tx3 = s_tx[3];
        s_last_tx4 = s_tx[4];
        s_last_tx5 = s_tx[5];
    }
    if (changed != 0)
    {
        log_tx_frame();
    }
}

static void apply_lost(unsigned char lost)
{
    if (s_lost == lost)
    {
        return;
    }
    s_lost = lost;
    hmi_set_pwr_lost(lost);
    if (lost != 0)
    {
        log_puts("PWR lost\r\n");
    }
    else
    {
        log_puts("PWR ok\r\n");
    }
}

static void apply_fault(unsigned char fault)
{
    if (s_fault == fault)
    {
        return;
    }
    s_fault = fault;
    hmi_set_pwr_fault(fault);
}

static void log_rx_frame(void)
{
    log_puts("PWR rx ver=");
    log_u16((unsigned int)s_rx_ver);
    log_puts(" type=");
    log_u16((unsigned int)s_rx_type);
    log_puts(" fault=");
    log_u16((unsigned int)s_rx_fault);
    log_puts("\r\n");
}

static void take_rx_frame(void)
{
    unsigned char sum;

    sum = checksum(s_rx, 32);
    if (sum != s_rx[32])
    {
        return;
    }

    s_lost_ms = 0;
    apply_lost(0);
    s_rx_ver = s_rx[2];
    s_rx_type = s_rx[3];
    s_rx_fault = s_rx[9];
    apply_fault(s_rx_fault);
    s_have_rx = 1;

    if ((s_rx_ver != s_last_ver) || (s_rx_type != s_last_type) || (s_rx_fault != s_last_fault_log))
    {
        s_last_ver = s_rx_ver;
        s_last_type = s_rx_type;
        s_last_fault_log = s_rx_fault;
        log_rx_frame();
    }
}

static void rx_byte(unsigned char b)
{
    if (s_st == ST_55)
    {
        if (b == 0x55)
        {
            s_rx[0] = b;
            s_st = ST_5A;
        }
    }
    else if (s_st == ST_5A)
    {
        if (b == 0x5A)
        {
            s_rx[1] = b;
            s_n = 2;
            s_st = ST_DAT;
        }
        else if (b == 0x55)
        {
            s_rx[0] = b;
        }
        else
        {
            s_st = ST_55;
        }
    }
    else
    {
        s_rx[s_n] = b;
        s_n++;
        if (s_n >= PWR_RX_N)
        {
            take_rx_frame();
            s_st = ST_55;
            s_n = 0;
        }
    }
}

void pwr_link_init(void)
{
    unsigned char i;

    for (i = 0; i < PWR_TX_N; i++)
    {
        s_tx[i] = 0;
    }
    s_st = ST_55;
    s_n = 0;
    s_lost = 0;
    s_fault = 0;
    s_have_rx = 0;
    s_last_tx2 = 0xFF;
    s_last_tx3 = 0xFF;
    s_last_tx4 = 0xFF;
    s_last_tx5 = 0xFF;
    s_last_ver = 0xFF;
    s_last_type = 0xFF;
    s_last_fault_log = 0xFF;
    s_tx_ms = 0;
    s_lost_ms = 0;
    pwr_uart_init();
    log_puts("PWR USCI2 4800 P4.4/P4.5 period=150ms cur=set until NTC\r\n");
    send_tx();
}

void pwr_link_poll(void)
{
    unsigned char b;
    unsigned char n;

    n = 0;
    while ((n < 16U) && (pwr_uart_getc(&b) != 0))
    {
        rx_byte(b);
        n++;
    }

    s_tx_ms++;
    if (s_tx_ms >= PWR_TX_MS)
    {
        s_tx_ms = 0;
        send_tx();
    }

    if (s_lost_ms < PWR_LOST_MS)
    {
        s_lost_ms++;
        if (s_lost_ms >= PWR_LOST_MS)
        {
            apply_lost(1);
        }
    }
}

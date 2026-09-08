#include "board.h"
#include "tuya_link.h"
#include "wifi_uart.h"
#include "hmi.h"
#include "ntc.h"
#include "log_uart.h"
#include "ota_dl.h"

/*
 * Official Tuya UART (copied MCU_SDK v2.6.2 headers): 55 AA, PID
 * uforlynlgj5xx3zg, DP 1/2/3/4/5/19/22/23/24/25/120/150.
 * Full protocol.c is not C51-safe (weak, u32=int, size). This file is the
 * heartbeat / product / state / DP subset those headers describe.
 */

#define FRAME_FIRST    0x55
#define FRAME_SECOND   0xAA
#define MCU_TX_VER     0x03
#define MCU_RX_VER     0x00

#define CMD_HEART      0
#define CMD_PRODUCT    1
#define CMD_WORKMODE   2
#define CMD_WIFISTATE  3
#define CMD_RESET      4
#define CMD_WIFIMODE   5
#define CMD_DP_DOWN    6
#define CMD_DP_UP      7
#define CMD_QUERY      8
#define CMD_OTA_START  0x0A
#define CMD_OTA_TRANS  0x0B

#define DP_SWITCH      1
#define DP_TEMP_SET    2
#define DP_TEMP_CUR    3
#define DP_MODE        4
#define DP_FAN         5
#define DP_UNIT        19
#define DP_FAULT       22
#define DP_TEMP_CUR_F  23
#define DP_TEMP_SET_F  24
#define DP_SLEEP       25
#define DP_SAVER       120
#define DP_USAGE       150

#define TY_BOOL        0x01
#define TY_VALUE       0x02
#define TY_ENUM        0x04
#define TY_FAULT       0x05

#define ST_55          0
#define ST_AA          1
#define ST_VER         2
#define ST_CMD         3
#define ST_LH          4
#define ST_LL          5
#define ST_DATA        6
#define ST_CS          7

#define DATA_MAX       260

#define WIFI_SMART     0x00
#define WIFI_AP        0x01
#define WIFI_NOT_CONN  0x02
#define WIFI_CONN      0x03
#define WIFI_CLOUD     0x04
#define WIFI_LOWPWR    0x05
#define WIFI_SMART_AP  0x06
#define WIFI_UNKNOWN   0xFF

static char code s_prod[] = "{\"p\":\"uforlynlgj5xx3zg\",\"v\":\"1.2.13\",\"m\":2}";

static unsigned char xdata s_data[DATA_MAX];
static unsigned char xdata s_tx[16];
static unsigned char xdata s_st;
static unsigned char xdata s_cmd;
static unsigned char xdata s_ver;
static unsigned char xdata s_sum;
static unsigned int xdata s_len;
static unsigned int xdata s_got;
static unsigned char xdata s_hb_done;
static unsigned char xdata s_wifi_st;
static unsigned char xdata s_logged_st;
static unsigned char xdata s_got_rx;
static unsigned char xdata s_got_hb;
static unsigned char xdata s_need_rst;
static unsigned char xdata s_rst_n;
static unsigned int xdata s_rst_ms;
static unsigned int xdata s_norx_ms;
static unsigned char xdata s_tx_fail;
static unsigned long xdata s_firm_len;
static unsigned char xdata s_ota_on;

static unsigned char send_byte(unsigned char b)
{
    if (wifi_uart_putc(b) == 0)
    {
        s_tx_fail = 1;
        return 0;
    }
    return 1;
}

static void send_frame(unsigned char cmd, unsigned char *buf, unsigned char n)
{
    unsigned char cs;
    unsigned char i;

    s_tx_fail = 0;
    send_byte(FRAME_FIRST);
    send_byte(FRAME_SECOND);
    send_byte(MCU_TX_VER);
    send_byte(cmd);
    send_byte(0);
    send_byte(n);
    cs = (unsigned char)(FRAME_FIRST + FRAME_SECOND + MCU_TX_VER + cmd + n);
    for (i = 0; i < n; i++)
    {
        send_byte(buf[i]);
        cs = (unsigned char)(cs + buf[i]);
    }
    send_byte(cs);
    if (s_tx_fail != 0)
    {
        log_puts("WIFI txto\r\n");
    }
}

static void send_str(unsigned char cmd, char code *s)
{
    unsigned char cs;
    unsigned char n;
    char code *p;

    n = 0;
    p = s;
    while (*p != 0)
    {
        n++;
        p++;
    }
    s_tx_fail = 0;
    send_byte(FRAME_FIRST);
    send_byte(FRAME_SECOND);
    send_byte(MCU_TX_VER);
    send_byte(cmd);
    send_byte(0);
    send_byte(n);
    cs = (unsigned char)(FRAME_FIRST + FRAME_SECOND + MCU_TX_VER + cmd + n);
    while (*s != 0)
    {
        send_byte((unsigned char)*s);
        cs = (unsigned char)(cs + (unsigned char)*s);
        s++;
    }
    send_byte(cs);
    if (s_tx_fail != 0)
    {
        log_puts("WIFI txto\r\n");
    }
}

static void report_bool(unsigned char dpid, unsigned char v)
{
    s_tx[0] = dpid;
    s_tx[1] = TY_BOOL;
    s_tx[2] = 0;
    s_tx[3] = 1;
    s_tx[4] = (unsigned char)((v != 0) ? 1 : 0);
    send_frame(CMD_DP_UP, s_tx, 5);
}

static void report_enum(unsigned char dpid, unsigned char v)
{
    s_tx[0] = dpid;
    s_tx[1] = TY_ENUM;
    s_tx[2] = 0;
    s_tx[3] = 1;
    s_tx[4] = v;
    send_frame(CMD_DP_UP, s_tx, 5);
}

static void report_value(unsigned char dpid, unsigned int v)
{
    s_tx[0] = dpid;
    s_tx[1] = TY_VALUE;
    s_tx[2] = 0;
    s_tx[3] = 4;
    s_tx[4] = 0;
    s_tx[5] = 0;
    s_tx[6] = (unsigned char)(v >> 8);
    s_tx[7] = (unsigned char)v;
    send_frame(CMD_DP_UP, s_tx, 8);
}

static void report_fault(unsigned char bits)
{
    s_tx[0] = DP_FAULT;
    s_tx[1] = TY_FAULT;
    s_tx[2] = 0;
    s_tx[3] = 1;
    s_tx[4] = bits;
    send_frame(CMD_DP_UP, s_tx, 5);
}

static unsigned int ntc_f(void)
{
    return ((unsigned int)ntc_c() * 9U) / 5U + 32U;
}

static void report_temps(void)
{
    if (ntc_ready() != 0)
    {
        report_value(DP_TEMP_CUR, ntc_c());
        report_value(DP_TEMP_CUR_F, ntc_f());
    }
}

static void report_usage(void)
{
    report_value(DP_USAGE, hmi_usage_min());
}

static void report_all(void)
{
    unsigned char unit_f;

    unit_f = hmi_unit_f();
    report_bool(DP_SWITCH, hmi_power());
    report_value(DP_TEMP_SET, hmi_setpoint_c());
    report_temps();
    report_enum(DP_MODE, hmi_mode_tuya());
    report_enum(DP_FAN, hmi_fan());
    report_enum(DP_UNIT, unit_f);
    report_fault(hmi_fault_bits());
    if (unit_f != 0)
    {
        report_value(DP_TEMP_SET_F, hmi_setpoint_disp());
    }
    else
    {
        report_value(DP_TEMP_SET_F, ((unsigned int)hmi_setpoint_c() * 9U + 2U) / 5U + 32U);
    }
    report_bool(DP_SLEEP, hmi_sleep());
    report_bool(DP_SAVER, hmi_saver());
    report_usage();
}

static void apply_dp(unsigned char dpid, unsigned char type, unsigned char *v, unsigned int n)
{
    unsigned int val;

    if (n == 0)
    {
        return;
    }
    if ((type == TY_VALUE) && (n >= 4U))
    {
        val = ((unsigned int)v[2] << 8) | v[3];
    }
    else
    {
        val = v[0];
    }

    if (dpid == DP_SWITCH)
    {
        hmi_wifi_set_power((unsigned char)val);
        report_bool(DP_SWITCH, hmi_power());
        report_usage();
    }
    else if (dpid == DP_TEMP_SET)
    {
        hmi_wifi_set_temp_c((unsigned char)val);
        report_value(DP_TEMP_SET, hmi_setpoint_c());
    }
    else if (dpid == DP_MODE)
    {
        hmi_wifi_set_mode_tuya((unsigned char)val);
        report_enum(DP_MODE, hmi_mode_tuya());
    }
    else if (dpid == DP_FAN)
    {
        hmi_wifi_set_fan((unsigned char)val);
        report_enum(DP_FAN, hmi_fan());
    }
    else if (dpid == DP_UNIT)
    {
        hmi_wifi_set_unit((unsigned char)val);
        report_enum(DP_UNIT, hmi_unit_f());
    }
    else if (dpid == DP_TEMP_SET_F)
    {
        hmi_wifi_set_temp_f((unsigned char)val);
        report_value(DP_TEMP_SET_F, (hmi_unit_f() != 0) ? hmi_setpoint_disp() : val);
    }
    else if (dpid == DP_SLEEP)
    {
        hmi_wifi_set_sleep((unsigned char)val);
        report_bool(DP_SLEEP, hmi_sleep());
    }
    else if (dpid == DP_SAVER)
    {
        hmi_wifi_set_saver((unsigned char)val);
        report_bool(DP_SAVER, hmi_saver());
    }
}

static void handle_dp_down(void)
{
    unsigned int i;
    unsigned int dplen;
    unsigned char dpid;
    unsigned char type;

    i = 0;
    while ((i + 4U) <= s_len)
    {
        dpid = s_data[i];
        type = s_data[i + 1U];
        dplen = ((unsigned int)s_data[i + 2U] << 8) | s_data[i + 3U];
        if ((i + 4U + dplen) > s_len)
        {
            break;
        }
        apply_dp(dpid, type, &s_data[i + 4U], dplen);
        i = i + 4U + dplen;
    }
}

static void handle_frame(void)
{
    unsigned char hb;

    if ((s_ver != MCU_RX_VER) && (s_ver != MCU_TX_VER))
    {
        return;
    }

    if (s_cmd == CMD_HEART)
    {
        if (s_got_hb == 0)
        {
            s_got_hb = 1;
            log_puts("WIFI hb\r\n");
        }
        hb = (unsigned char)((s_hb_done != 0) ? 1 : 0);
        s_hb_done = 1;
        send_frame(CMD_HEART, &hb, 1);
    }
    else if (s_cmd == CMD_PRODUCT)
    {
        send_str(CMD_PRODUCT, s_prod);
    }
    else if (s_cmd == CMD_WORKMODE)
    {
        send_frame(CMD_WORKMODE, s_data, 0);
    }
    else if (s_cmd == CMD_WIFISTATE)
    {
        if (s_len != 0)
        {
            s_wifi_st = s_data[0];
            if (s_wifi_st != s_logged_st)
            {
                s_logged_st = s_wifi_st;
                log_puts("WIFI st=");
                log_u16((unsigned int)s_wifi_st);
                log_puts("\r\n");
            }
        }
        send_frame(CMD_WIFISTATE, s_data, 0);
        if ((s_wifi_st <= 1U) || (s_wifi_st == WIFI_SMART_AP))
        {
            s_need_rst = 0;
        }
    }
    else if (s_cmd == CMD_RESET)
    {
        s_need_rst = 0;
        log_puts("WIFI rst ok\r\n");
    }
    else if (s_cmd == CMD_DP_DOWN)
    {
        if (s_ota_on == 0)
        {
            handle_dp_down();
        }
    }
    else if (s_cmd == CMD_QUERY)
    {
        if (s_ota_on == 0)
        {
            report_all();
        }
    }
    else if (s_cmd == CMD_OTA_START)
    {
        unsigned long n;
        unsigned char psz;

        if (s_len < 4U)
        {
            return;
        }
        n = ((unsigned long)s_data[0] << 24);
        n |= ((unsigned long)s_data[1] << 16);
        n |= ((unsigned long)s_data[2] << 8);
        n |= (unsigned long)s_data[3];
        s_firm_len = n;
        s_ota_on = ota_begin(n);
        psz = 0;
        send_frame(CMD_OTA_START, &psz, 1);
    }
    else if (s_cmd == CMD_OTA_TRANS)
    {
        unsigned long pos;
        unsigned int chunk;
        unsigned char ok;

        if (s_ota_on == 0)
        {
            return;
        }
        if (s_len < 4U)
        {
            return;
        }
        pos = ((unsigned long)s_data[0] << 24);
        pos |= ((unsigned long)s_data[1] << 16);
        pos |= ((unsigned long)s_data[2] << 8);
        pos |= (unsigned long)s_data[3];
        if ((s_len == 4U) && (pos == s_firm_len))
        {
            ok = ota_finish(s_firm_len);
            if (ok != 0)
            {
                send_frame(CMD_OTA_TRANS, s_data, 0);
            }
            return;
        }
        chunk = (unsigned int)(s_len - 4U);
        ok = ota_write(pos, &s_data[4], chunk);
        if (ok != 0)
        {
            send_frame(CMD_OTA_TRANS, s_data, 0);
        }
    }
}

static void rx_byte(unsigned char b)
{
    if (s_st == ST_55)
    {
        if (b == FRAME_FIRST)
        {
            s_sum = b;
            s_st = ST_AA;
        }
    }
    else if (s_st == ST_AA)
    {
        if (b == FRAME_SECOND)
        {
            s_sum = (unsigned char)(s_sum + b);
            s_st = ST_VER;
        }
        else if (b != FRAME_FIRST)
        {
            s_st = ST_55;
        }
    }
    else if (s_st == ST_VER)
    {
        s_ver = b;
        s_sum = (unsigned char)(s_sum + b);
        s_st = ST_CMD;
    }
    else if (s_st == ST_CMD)
    {
        s_cmd = b;
        s_sum = (unsigned char)(s_sum + b);
        s_st = ST_LH;
    }
    else if (s_st == ST_LH)
    {
        s_len = (unsigned int)b << 8;
        s_sum = (unsigned char)(s_sum + b);
        s_st = ST_LL;
    }
    else if (s_st == ST_LL)
    {
        s_len |= b;
        s_sum = (unsigned char)(s_sum + b);
        s_got = 0;
        if (s_len > DATA_MAX)
        {
            s_st = ST_55;
        }
        else if (s_len == 0)
        {
            s_st = ST_CS;
        }
        else
        {
            s_st = ST_DATA;
        }
    }
    else if (s_st == ST_DATA)
    {
        s_data[s_got] = b;
        s_sum = (unsigned char)(s_sum + b);
        s_got++;
        if (s_got >= s_len)
        {
            s_st = ST_CS;
        }
    }
    else
    {
        if (b == s_sum)
        {
            handle_frame();
        }
        s_st = ST_55;
    }
}

static void do_reset(void)
{
    s_tx[0] = 1;
    send_frame(CMD_RESET, s_data, 0);
    send_frame(CMD_WIFIMODE, s_tx, 1);
}

void tuya_link_init(void)
{
    wifi_uart_init();
    s_st = ST_55;
    s_hb_done = 0;
    s_wifi_st = WIFI_UNKNOWN;
    s_logged_st = WIFI_UNKNOWN;
    s_got_rx = 0;
    s_got_hb = 0;
    s_need_rst = 0;
    s_rst_n = 0;
    s_rst_ms = 0;
    s_norx_ms = 0;
    s_firm_len = 0;
    s_ota_on = 0;
}

void tuya_link_reset_wifi(void)
{
    log_puts("WIFI reset\r\n");
    s_need_rst = 1;
    s_rst_n = 0;
    s_rst_ms = 0;
    do_reset();
}

unsigned char tuya_link_wifi_state(void)
{
    return s_wifi_st;
}

void tuya_link_poll(void)
{
    unsigned char b;
    unsigned char rpt;

    wifi_uart_pins();
    wifi_uart_poll_rx();
    while (wifi_uart_rx_take(&b) != 0)
    {
        if (s_got_rx == 0)
        {
            s_got_rx = 1;
            log_puts("WIFI rx=");
            log_hex8(b);
            log_puts("\r\n");
        }
        rx_byte(b);
    }

    if (s_got_rx == 0)
    {
        if (s_norx_ms < 60000U)
        {
            s_norx_ms++;
        }
        if (s_norx_ms == 2000U)
        {
            log_puts("WIFI norx p20=");
            log_u16((unsigned int)P20);
            log_puts(" p21=");
            log_u16((unsigned int)P21);
            log_puts(" p26=");
            log_u16((unsigned int)P26);
            log_puts(" scon=");
            log_hex8(SCON);
            log_puts("\r\n");
        }
    }

    if (s_need_rst != 0)
    {
        s_rst_ms++;
        if (s_rst_ms >= 300U)
        {
            s_rst_ms = 0;
            do_reset();
            s_rst_n++;
            if (s_rst_n >= 6U)
            {
                s_need_rst = 0;
            }
        }
    }

    rpt = 0;
    if (ota_busy() == 0)
    {
        rpt = hmi_wifi_take_rpt();
    }
    if ((rpt & HMI_RPT_FULL) != 0)
    {
        report_all();
    }
    else
    {
        if ((rpt & HMI_RPT_USAGE) != 0)
        {
            report_usage();
        }
        if ((rpt & HMI_RPT_TEMP) != 0)
        {
            report_temps();
        }
    }
}

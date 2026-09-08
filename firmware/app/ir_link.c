#include "board.h"
#include "ir_link.h"
#include "ir_rx.h"
#include "hmi.h"
#include "log_uart.h"

#define IR_SYS  0x56

static unsigned char xdata s_buf[IR_FRAME_N];
static unsigned char xdata s_last_a;
static unsigned char xdata s_last_d;
static unsigned char xdata s_last_e;
static unsigned char xdata s_last_i6;
static unsigned char xdata s_last_i7;
static unsigned char xdata s_have_last;

static unsigned char bitrev8(unsigned char v)
{
    unsigned char r;
    unsigned char i;

    r = 0;
    for (i = 0; i < 8U; i++)
    {
        r = (unsigned char)((r << 1) | (v & 1U));
        v >>= 1;
    }
    return r;
}

static unsigned char nibble_sum(unsigned char *b, unsigned char n)
{
    unsigned int s;
    unsigned char i;

    s = 0;
    for (i = 0; i < n; i++)
    {
        s = s + (unsigned int)((b[i] >> 4) & 0x0FU) + (unsigned int)(b[i] & 0x0FU);
    }
    return (unsigned char)s;
}

static void log_unsupported(unsigned char *b)
{
    unsigned char c;
    unsigned char e;
    unsigned char f;
    unsigned char g;

    c = b[3];
    e = b[5];
    f = b[6];
    g = b[7];
    if (((c & 0x01U) != 0) || ((e & 0x3FU) != 0) || ((f & 0xEFU) != 0) || ((g & 0x7FU) != 0))
    {
        log_puts("IR extra C=");
        log_hex8(c);
        log_puts(" E=");
        log_hex8(e);
        log_puts(" F=");
        log_hex8(f);
        log_puts(" G=");
        log_hex8(g);
        log_puts("\r\n");
    }
}

void ir_link_init(void)
{
    s_have_last = 0;
    ir_rx_init();
}

void ir_link_poll(void)
{
    unsigned char i;
    unsigned char order_msb;
    unsigned char a;
    unsigned char d;
    unsigned char e;
    unsigned char h;
    unsigned char ii;
    unsigned char j;
    unsigned char k;
    unsigned char mode_bits;
    unsigned char mode_ok;
    unsigned char mode;
    unsigned char fan_bits;
    unsigned char fan;
    unsigned char power;
    unsigned char set_c;
    unsigned char f_plus;
    unsigned char unit_f;
    unsigned char tmr_op;
    unsigned char tmr_h;
    unsigned char on_h;
    unsigned char off_h;

    if (ir_rx_take(s_buf) == 0)
    {
        return;
    }

    order_msb = 0;
    if (s_buf[0] == bitrev8(IR_SYS))
    {
        order_msb = 1;
        for (i = 0; i < IR_FRAME_N; i++)
        {
            s_buf[i] = bitrev8(s_buf[i]);
        }
    }

    if (s_buf[0] != IR_SYS)
    {
        log_puts("IR drop sys=");
        log_hex8(s_buf[0]);
        log_puts("\r\n");
        return;
    }

    if (nibble_sum(s_buf, 14) != s_buf[14])
    {
        log_puts("IR bad sum got=");
        log_hex8(s_buf[14]);
        log_puts(" exp=");
        log_hex8(nibble_sum(s_buf, 14));
        log_puts("\r\n");
        return;
    }

    a = s_buf[1];
    d = s_buf[4];
    e = s_buf[5];
    h = s_buf[8];
    ii = s_buf[9];
    j = s_buf[10];
    k = s_buf[11];

    f_plus = (unsigned char)((ii >> 6) & 0x01U);
    unit_f = (unsigned char)((ii >> 7) & 0x01U);

    if ((s_have_last != 0) && (a == s_last_a) && (d == s_last_d) && (e == s_last_e)
        && (f_plus == s_last_i6) && (unit_f == s_last_i7))
    {
        return;
    }
    s_last_a = a;
    s_last_d = d;
    s_last_e = e;
    s_last_i6 = f_plus;
    s_last_i7 = unit_f;
    s_have_last = 1;

    power = (unsigned char)(((e & 0xC0U) == 0) ? 1 : 0);
    mode_bits = (unsigned char)((d >> 4) & 0x07U);
    mode_ok = 1;
    mode = 0;
    if (mode_bits == 2U)
    {
        mode = 0;
    }
    else if (mode_bits == 1U)
    {
        mode = 2;
    }
    else if (mode_bits == 3U)
    {
        mode = 1;
    }
    else if (mode_bits == 5U)
    {
        mode = 3;
    }
    else
    {
        mode_ok = 0;
    }

    fan_bits = (unsigned char)(d & 0x03U);
    if (fan_bits == 2U)
    {
        fan = 0;
    }
    else if (fan_bits == 3U)
    {
        fan = 1;
    }
    else if (fan_bits == 1U)
    {
        fan = 2;
    }
    else
    {
        fan = 0xFF;
    }

    if (a < 0x6CU)
    {
        set_c = 16;
    }
    else
    {
        set_c = (unsigned char)(a - 0x5CU);
        if (set_c > 31U)
        {
            set_c = 31;
        }
    }

    on_h = (unsigned char)(h & 0x1FU);
    off_h = (unsigned char)(j & 0x1FU);
    tmr_op = 0;
    tmr_h = 0;
    if (((d & 0x0CU) == 0) && (on_h == 0) && (off_h == 0) && ((k & 0x3FU) == 0) && ((ii & 0x3FU) == 0))
    {
        tmr_op = 3;
    }
    else if (((d & 0x0CU) == 0x0CU) || (((d & 0x04U) != 0) && ((d & 0x08U) != 0)))
    {
        log_puts("IR tmr both D=");
        log_hex8(d);
        log_puts("\r\n");
    }
    else if ((d & 0x04U) != 0)
    {
        if ((off_h >= 1U) && (off_h <= 24U))
        {
            tmr_op = 1;
            tmr_h = off_h;
        }
        else
        {
            log_puts("IR tmr off-min only\r\n");
        }
    }
    else if ((d & 0x08U) != 0)
    {
        if ((on_h >= 1U) && (on_h <= 24U))
        {
            tmr_op = 2;
            tmr_h = on_h;
        }
        else
        {
            log_puts("IR tmr on-min only\r\n");
        }
    }

    log_puts("IR ok bitorder=");
    if (order_msb != 0)
    {
        log_puts("msb");
    }
    else
    {
        log_puts("lsb");
    }
    log_puts(" p=");
    log_u16((unsigned int)power);
    log_puts(" mbits=");
    log_u16((unsigned int)mode_bits);
    log_puts(" fbits=");
    log_u16((unsigned int)fan_bits);
    log_puts(" A=");
    log_hex8(a);
    log_puts(" E=");
    log_hex8(e);
    log_puts("\r\n");

    if (mode_ok == 0)
    {
        log_puts("IR auto/unknown mode ignore\r\n");
    }
    if (fan == 0xFF)
    {
        log_puts("IR fan=off keep\r\n");
    }
    log_puts("IR unit=");
    if (unit_f != 0)
    {
        log_puts("F");
    }
    else
    {
        log_puts("C");
    }
    if (f_plus != 0)
    {
        log_puts(" F+1");
    }
    log_puts("\r\n");
    if ((j & 0x80U) != 0)
    {
        log_puts("IR fan+1 flag log-only\r\n");
    }

    log_unsupported(s_buf);

    hmi_apply_ir(power, mode_ok, mode, fan, set_c, f_plus, unit_f, tmr_op, tmr_h);
}

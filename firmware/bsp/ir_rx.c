#include "board.h"
#include "ir_rx.h"

/*
 * WH1738 on P3.2 (SOP28 pin 12, net P_REC). Idle high, carrier = low.
 * P3.2 has no INT0/1/2 (datasheet SOP28). Timer1 mode 1 @ Fsys samples
 * every 50 us. Leader/bit windows are typical 38 kHz Gree mark/space
 * (protocol .doc timing cells were empty). Do not copy old ADB timings.
 *
 * Timer0 stays the 1 ms poll timebase. TMCON.1 (T1FD) = Fsys.
 */

#define T1_RELOAD     ((unsigned int)(65536U - 1600U))
#define T_LEAD_MIN    140U
#define T_LEAD_MAX    240U
#define T_LSPC_MIN    60U
#define T_LSPC_MAX    140U
#define T_MARK_MIN    6U
#define T_MARK_MAX    24U
#define T_BIT0_MAX    18U
#define T_BIT1_MIN    24U
#define T_BIT1_MAX    50U
#define T_IDLE_MAX    80U

static unsigned char data s_prev;
static unsigned char data s_have_lead;
static unsigned char data s_bitn;
static unsigned char data s_acc;
static unsigned char data s_ready;
static unsigned int data s_w;
static unsigned char xdata s_raw[IR_FRAME_N];
static unsigned char xdata s_frame[IR_FRAME_N];

static void ir_reset(void)
{
    s_have_lead = 0;
    s_bitn = 0;
    s_acc = 0;
    s_w = 0;
}

static void ir_store_bit(unsigned char one)
{
    if (s_bitn >= 120U)
    {
        return;
    }
    if (one != 0)
    {
        s_acc = (unsigned char)(s_acc | (unsigned char)(1U << (s_bitn & 7U)));
    }
    s_bitn++;
    if ((s_bitn & 7U) == 0)
    {
        s_raw[(unsigned char)((s_bitn - 1U) >> 3)] = s_acc;
        s_acc = 0;
    }
}

static void ir_finish(void)
{
    unsigned char i;

    if ((s_bitn >= 120U) && (s_ready == 0))
    {
        for (i = 0; i < IR_FRAME_N; i++)
        {
            s_frame[i] = s_raw[i];
        }
        s_ready = 1;
    }
    ir_reset();
}

static void ir_on_pulse(unsigned char lvl, unsigned int w)
{
    if (lvl == 0)
    {
        if ((w >= T_LEAD_MIN) && (w <= T_LEAD_MAX))
        {
            s_have_lead = 1;
            s_bitn = 0;
            s_acc = 0;
            return;
        }
        if ((s_have_lead != 0) && (w >= T_MARK_MIN) && (w <= T_MARK_MAX))
        {
            return;
        }
        ir_reset();
        return;
    }

    if (s_have_lead == 0)
    {
        return;
    }

    if ((s_bitn == 0) && (w >= T_LSPC_MIN) && (w <= T_LSPC_MAX))
    {
        return;
    }

    if (s_bitn >= 120U)
    {
        ir_finish();
        return;
    }

    if ((w >= T_MARK_MIN) && (w <= T_BIT0_MAX))
    {
        ir_store_bit(0);
    }
    else if ((w >= T_BIT1_MIN) && (w <= T_BIT1_MAX))
    {
        ir_store_bit(1);
    }
    else
    {
        ir_reset();
        return;
    }

}

void ir_rx_init(void)
{
    ir_reset();
    s_ready = 0;
    s_prev = 1;

    P3CON &= ~0x04;
    P3PH |= 0x04;

    TMCON |= 0x02;
    TMOD = (TMOD & 0x0F) | 0x10;
    TL1 = (unsigned char)(T1_RELOAD & 0xFF);
    TH1 = (unsigned char)(T1_RELOAD >> 8);
    IPT1 = 1;
    ET1 = 1;
    TR1 = 1;
}

unsigned char ir_rx_take(unsigned char *dst)
{
    unsigned char i;
    unsigned char ea;

    if (s_ready == 0)
    {
        return 0;
    }
    ea = EA;
    EA = 0;
    for (i = 0; i < IR_FRAME_N; i++)
    {
        dst[i] = s_frame[i];
    }
    s_ready = 0;
    EA = ea;
    return 1;
}

void ir_tm1_isr(void) interrupt 3
{
    unsigned char now;

    TL1 = (unsigned char)(T1_RELOAD & 0xFF);
    TH1 = (unsigned char)(T1_RELOAD >> 8);

    now = IR_PIN;
    if (now == s_prev)
    {
        if (s_w < 2000U)
        {
            s_w++;
        }
        if ((now != 0) && (s_w >= T_IDLE_MAX) && (s_have_lead != 0))
        {
            if (s_bitn >= 120U)
            {
                ir_finish();
            }
            else if (s_bitn != 0)
            {
                ir_reset();
            }
        }
        return;
    }

    ir_on_pulse(s_prev, s_w);
    s_prev = now;
    s_w = 1;
}

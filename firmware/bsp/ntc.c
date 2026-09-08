#include "board.h"
#include "ntc.h"
#include "log_uart.h"

/*
 * Indoor NTC on P2.3 / AIN7 (SOP28 pin 22, PcbDoc net AD_NTC).
 * ADC registers follow official 8763 ADC_Init.c (ADCCON/ADCCFG/12-bit
 * result). Poll EOC bit5; do not enable EADC (Timer1 + TK already use ISRs).
 *
 * 10K lookup: user-supplied c_AD_TO_TEMPER_10K_TABLE. Index is °C (0–72),
 * value is 8-bit AD. Board ADC rises with temperature (hand-warmed 0.1.2
 * went down after invert), so lookup is adc>>4 against the table as written.
 */

#define NTC_CH        7U
#define NTC_OPEN_MAX  512U
#define NTC_SHORT_MIN 3200U
#define NTC_SAMPLE_MS 100U
#define NTC_PUB_MS    1000U
#define NTC_T_MAX     72U

static unsigned char code s_ad8[73] = {
    40,  41,  43,  44,  46,  48,  49,  51,  53,  54,
    56,  58,  60,  62,  64,  66,  68,  70,  72,  74,
    76,  78,  80,  82,  84,  86,  88,  90,  93,  95,
    97,  99,  101, 104, 106, 108, 110, 112, 115, 117,
    119, 121, 123, 126, 128, 130, 132, 134, 136, 138,
    140, 142, 145, 147, 149, 150, 152, 154, 156, 158,
    160, 162, 164, 165, 167, 169, 171, 172, 174, 176,
    177, 179, 180
};

static unsigned int xdata s_filt;
static unsigned int xdata s_ms;
static unsigned int xdata s_pub_ms;
static unsigned char xdata s_ready;
static unsigned char xdata s_c;
static unsigned char xdata s_bad_logged;
static unsigned char xdata s_have_filt;

static unsigned char adc_to_c(unsigned int adc)
{
    unsigned char ad8;
    unsigned char i;

    ad8 = (unsigned char)(adc >> 4);
    if (ad8 <= s_ad8[0])
    {
        return 0;
    }
    if (ad8 >= s_ad8[NTC_T_MAX])
    {
        return NTC_T_MAX;
    }
    for (i = 0; i < NTC_T_MAX; i++)
    {
        if (ad8 < s_ad8[i + 1])
        {
            return i;
        }
    }
    return NTC_T_MAX;
}

static unsigned int adc_read(void)
{
    unsigned int t;
    unsigned int v;

    ADCCON |= 0x40;
    t = 2000U;
    while (((ADCCON & 0x20) == 0) && (t != 0))
    {
        t--;
    }
    v = ((unsigned int)ADCVH << 4) | ((unsigned int)ADCVL >> 4);
    ADCCON &= ~0x20;
    if (t == 0)
    {
        return 0xFFFF;
    }
    return v;
}

static void log_ntc(unsigned int adc)
{
    log_puts("NTC c=");
    log_u16((unsigned int)s_c);
    log_puts(" ad=");
    log_u16(adc);
    log_puts("\r\n");
}

void ntc_init(void)
{
    s_filt = 2048;
    s_ms = 0;
    s_pub_ms = 0;
    s_ready = 0;
    s_c = 25;
    s_bad_logged = 0;
    s_have_filt = 0;

    P2CON &= ~0x08;
    P2PH &= ~0x08;
    ADCCFG0 = 0x80;
    ADCCFG1 = 0x00;
    ADCCFG2 = 0x10;
    ADCCON = (unsigned char)(0x80 | NTC_CH);
    EADC = 0;
}

void ntc_poll(void)
{
    unsigned int adc;
    unsigned char c;

    s_ms++;
    if (s_ms < NTC_SAMPLE_MS)
    {
        return;
    }
    s_ms = 0;

    adc = adc_read();
    if ((adc == 0xFFFF) || (adc < NTC_OPEN_MAX) || (adc > NTC_SHORT_MIN))
    {
        if (s_bad_logged == 0)
        {
            s_bad_logged = 1;
            log_puts("NTC bad adc=");
            log_u16(adc);
            log_puts("\r\n");
        }
        return;
    }
    s_bad_logged = 0;

    if (s_have_filt == 0)
    {
        s_filt = adc;
        s_have_filt = 1;
    }
    else
    {
        s_filt = (unsigned int)(((unsigned long)s_filt * 7UL + (unsigned long)adc) / 8UL);
    }

    if (s_ready == 0)
    {
        s_c = adc_to_c(s_filt);
        s_ready = 1;
        s_pub_ms = 0;
        log_ntc(s_filt);
        return;
    }

    s_pub_ms = (unsigned int)(s_pub_ms + NTC_SAMPLE_MS);
    if (s_pub_ms < NTC_PUB_MS)
    {
        return;
    }
    s_pub_ms = 0;

    c = adc_to_c(s_filt);
    if (c != s_c)
    {
        s_c = c;
        log_ntc(s_filt);
    }
}

unsigned char ntc_ready(void)
{
    return s_ready;
}

unsigned char ntc_c(void)
{
    return s_c;
}

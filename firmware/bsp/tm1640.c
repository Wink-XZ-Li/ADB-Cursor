#include "board.h"
#include "tm1640.h"

/*
 * TM1640 two-wire protocol from Titan TM1640 V1.5:
 * DIN changes only while SCLK is low; sampled on SCLK rising edge.
 * LSB first. Start: SCLK=1, DIN 1->0. Stop: SCLK=1, DIN 0->1.
 * Auto-inc write: 0x40, then 0xC0 + 16 data, then display-on 0x8F.
 *
 * Pins: SOP28 pin13=P3.1, pin14=P3.0. Panel responded on Stage1A 0.1.0.
 */

#define TM1640_SCLK   P31
#define TM1640_DIN    P30

static void tm1640_delay(void)
{
    unsigned char i;
    for (i = 0; i < 12; i++)
    {
        _nop_();
    }
}

static void tm1640_start(void)
{
    TM1640_DIN = 1;
    TM1640_SCLK = 1;
    tm1640_delay();
    TM1640_DIN = 0;
    tm1640_delay();
    TM1640_SCLK = 0;
    tm1640_delay();
}

static void tm1640_stop(void)
{
    TM1640_DIN = 0;
    TM1640_SCLK = 0;
    tm1640_delay();
    TM1640_SCLK = 1;
    tm1640_delay();
    TM1640_DIN = 1;
    tm1640_delay();
}

static void tm1640_write_byte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        TM1640_SCLK = 0;
        TM1640_DIN = (bit)(dat & 0x01);
        tm1640_delay();
        TM1640_SCLK = 1;
        tm1640_delay();
        dat >>= 1;
    }
    TM1640_SCLK = 0;
}

void tm1640_init(void)
{
    P3CON |= 0x03;
    P3PH  &= 0xFC;
    TM1640_SCLK = 1;
    TM1640_DIN = 1;
    tm1640_blank();
}

void tm1640_display(unsigned char *buf)
{
    unsigned char i;

    tm1640_start();
    tm1640_write_byte(0x40);
    tm1640_stop();

    tm1640_start();
    tm1640_write_byte(0xC0);
    for (i = 0; i < TM1640_GRID_COUNT; i++)
    {
        tm1640_write_byte(buf[i]);
    }
    tm1640_stop();

    tm1640_start();
    tm1640_write_byte(0x8F);
    tm1640_stop();
}

void tm1640_blank(void)
{
    unsigned char buf[16];
    unsigned char i;
    for (i = 0; i < 16; i++)
    {
        buf[i] = 0x00;
    }
    tm1640_display(buf);
}

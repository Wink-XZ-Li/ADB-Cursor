#include "board.h"
#include "timebase.h"
#include "beep.h"

/* User-confirmed: buzzer is P2.4. Spec: power-on beep only. */
#define BEEP_PIN  P24

static void beep_half(void)
{
    unsigned int i;
    /* ~200 us at 32 MHz, LARGE model (audible 2 kHz-class tone). */
    for (i = 0; i < 480; i++)
    {
        _nop_();
    }
}

void beep_init(void)
{
    P2CON |= 0x10;
    P2PH  &= 0xEF;
    BEEP_PIN = 0;
}

void beep_power_on(void)
{
    unsigned int n;
    for (n = 0; n < 300; n++)
    {
        BEEP_PIN = 1;
        beep_half();
        BEEP_PIN = 0;
        beep_half();
    }
}

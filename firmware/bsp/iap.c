#include "board.h"
#include "iap.h"
#include "ota_map.h"

#define IAP_APROM  0x00

static void iap_nops(void)
{
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

unsigned char iap_code_read(unsigned int addr)
{
    unsigned char v;
    unsigned char ea;
    unsigned char iapade;
    unsigned char rombnk;
    unsigned char code *p;

    p = 0;
    ea = EA;
    EA = 0;
    iapade = IAPADE;
    rombnk = ROMBNK;
    IAPADE = IAP_APROM;
    ROMBNK = (unsigned char)((ROMBNK & 0xCF) | 0x10);
    v = *(p + addr);
    IAPADE = iapade;
    ROMBNK = rombnk;
    EA = ea;
    return v;
}

void iap_code_erase(unsigned int addr)
{
    unsigned char ea;
    unsigned char iapade;
    unsigned char rombnk;

    if (addr < DL_BASE)
    {
        return;
    }
    ea = EA;
    EA = 0;
    iapade = IAPADE;
    rombnk = ROMBNK;
    IAPADE = IAP_APROM;
    ROMBNK = (unsigned char)((ROMBNK & 0xCF) | 0x10);
    IAPADH = (unsigned char)(addr >> 8);
    IAPADL = (unsigned char)addr;
    IAPKEY = 0xF0;
    IAPCTL = 0x20;
    IAPCTL |= 0x02;
    iap_nops();
    IAPADE = iapade;
    ROMBNK = rombnk;
    EA = ea;
    board_wdt_feed();
}

unsigned char iap_code_write(unsigned int addr, unsigned char v)
{
    unsigned char ea;
    unsigned char iapade;
    unsigned char rombnk;

    if (addr < DL_BASE)
    {
        return 0;
    }
    ea = EA;
    EA = 0;
    iapade = IAPADE;
    rombnk = ROMBNK;
    IAPADE = IAP_APROM;
    ROMBNK = (unsigned char)((ROMBNK & 0xCF) | 0x10);
    IAPDAT = v;
    IAPADH = (unsigned char)(addr >> 8);
    IAPADL = (unsigned char)addr;
    IAPKEY = 0xF0;
    IAPCTL = 0x10;
    IAPCTL |= 0x02;
    iap_nops();
    IAPADE = iapade;
    ROMBNK = rombnk;
    EA = ea;
    board_wdt_feed();
    return (unsigned char)((iap_code_read(addr) == v) ? 1 : 0);
}

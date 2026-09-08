#include "SC95F876x_C.H"
#include "intrins.h"
#include "boot_iap.h"

#define IAP_APROM   0x00
#define IAP_EEPROM  0x02

static void nops(void)
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

static void wdt(void)
{
    WDTCON |= 0x10;
}

static unsigned char movc_read(unsigned char area, unsigned int addr)
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
    IAPADE = area;
    ROMBNK = (unsigned char)((ROMBNK & 0xCF) | 0x10);
    v = *(p + addr);
    IAPADE = iapade;
    ROMBNK = rombnk;
    EA = ea;
    return v;
}

static void iap_go(unsigned char area, unsigned int addr, unsigned char ctl, unsigned char dat)
{
    unsigned char ea;
    unsigned char iapade;
    unsigned char rombnk;

    ea = EA;
    EA = 0;
    iapade = IAPADE;
    rombnk = ROMBNK;
    IAPADE = area;
    ROMBNK = (unsigned char)((ROMBNK & 0xCF) | 0x10);
    if ((ctl & 0x10) != 0)
    {
        IAPDAT = dat;
    }
    IAPADH = (unsigned char)(addr >> 8);
    IAPADL = (unsigned char)addr;
    IAPKEY = 0xF0;
    IAPCTL = ctl;
    IAPCTL |= 0x02;
    nops();
    IAPADE = iapade;
    ROMBNK = rombnk;
    EA = ea;
    wdt();
}

unsigned char boot_ee_read(unsigned int addr)
{
    return movc_read(IAP_EEPROM, addr);
}

void boot_ee_erase(unsigned int addr)
{
    iap_go(IAP_EEPROM, addr, 0x20, 0);
}

void boot_ee_write(unsigned int addr, unsigned char v)
{
    iap_go(IAP_EEPROM, addr, 0x10, v);
}

unsigned char boot_code_read(unsigned int addr)
{
    return movc_read(IAP_APROM, addr);
}

void boot_code_erase(unsigned int addr)
{
    if (addr < 0x1000U)
    {
        return;
    }
    iap_go(IAP_APROM, addr, 0x20, 0);
}

void boot_code_write(unsigned int addr, unsigned char v)
{
    if (addr < 0x1000U)
    {
        return;
    }
    iap_go(IAP_APROM, addr, 0x10, v);
}

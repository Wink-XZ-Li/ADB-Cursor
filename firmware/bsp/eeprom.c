#include "board.h"
#include "eeprom.h"

/*
 * Independent 6K EEPROM: IAPADE=0x02, 0x0000..0x17FF, 512-byte sectors.
 * Sequence from official 8763 Demo IAP_Read (area 0x02) plus 8617/8737
 * write/erase: IAPKEY=0xF0, IAPCTL=0x10 program / 0x20 erase, then |=0x02.
 * Does not touch APROM or Option.
 */

#define IAP_EEPROM  0x02
#define IAP_APROM   0x00

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

unsigned char eeprom_read(unsigned int addr)
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
    IAPADE = IAP_EEPROM;
    ROMBNK = (unsigned char)((ROMBNK & 0xCF) | 0x10);
    v = *(p + addr);
    IAPADE = iapade;
    ROMBNK = rombnk;
    EA = ea;
    return v;
}

void eeprom_erase_sector(unsigned int addr)
{
    unsigned char ea;
    unsigned char iapade;
    unsigned char rombnk;

    ea = EA;
    EA = 0;
    iapade = IAPADE;
    rombnk = ROMBNK;
    IAPADE = IAP_EEPROM;
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

void eeprom_write_byte(unsigned int addr, unsigned char v)
{
    unsigned char ea;
    unsigned char iapade;
    unsigned char rombnk;

    ea = EA;
    EA = 0;
    iapade = IAPADE;
    rombnk = ROMBNK;
    IAPADE = IAP_EEPROM;
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
}

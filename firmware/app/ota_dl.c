#include "board.h"
#include "ota_dl.h"
#include "ota_map.h"
#include "iap.h"
#include "eeprom.h"
#include "hmi.h"
#include "log_uart.h"

static unsigned char xdata s_busy;
static unsigned char xdata s_erased;

static unsigned int crc16_mem(unsigned int addr, unsigned int n)
{
    unsigned int c;
    unsigned int i;
    unsigned char b;
    unsigned char k;

    c = 0xFFFF;
    for (i = 0; i < n; i++)
    {
        b = iap_code_read((unsigned int)(addr + i));
        c = (unsigned int)(c ^ ((unsigned int)b << 8));
        for (k = 0; k < 8U; k++)
        {
            if ((c & 0x8000U) != 0)
            {
                c = (unsigned int)((c << 1) ^ 0x1021U);
            }
            else
            {
                c = (unsigned int)(c << 1);
            }
        }
        if ((i & 0x3FU) == 0)
        {
            board_wdt_feed();
        }
    }
    return c;
}

static void erase_dl(void)
{
    unsigned int a;
    unsigned char n;

    a = DL_BASE;
    n = 0;
    while (n < IAP_SEC_N_DL)
    {
        iap_code_erase(a);
        a = (unsigned int)(a + IAP_SEC);
        n++;
    }
}

static void write_magic(void)
{
    eeprom_erase_sector(OTA_MAGIC_EE);
    eeprom_write_byte(OTA_MAGIC_EE, OTA_MAGIC0);
    eeprom_write_byte((unsigned int)(OTA_MAGIC_EE + 1U), OTA_MAGIC1);
    eeprom_write_byte((unsigned int)(OTA_MAGIC_EE + 2U), OTA_MAGIC2);
    eeprom_write_byte((unsigned int)(OTA_MAGIC_EE + 3U), OTA_MAGIC3);
}

static void reboot_boot(void)
{
    EA = 0;
    IAPADE = 0x00;
    ((void (code *)(void)) 0)();
}

unsigned char ota_busy(void)
{
    return s_busy;
}

unsigned char ota_begin(unsigned long file_len)
{
    if ((file_len < OADB_HDR_N) || (file_len > DL_SIZE))
    {
        return 0;
    }
    s_busy = 1;
    s_erased = 0;
    hmi_set_ota(1);
    log_puts("OTA begin\r\n");
    erase_dl();
    s_erased = 1;
    return 1;
}

unsigned char ota_write(unsigned long pos, unsigned char *buf, unsigned int n)
{
    unsigned int addr;
    unsigned int i;

    if ((s_busy == 0) || (s_erased == 0))
    {
        return 0;
    }
    if ((pos + n) > DL_SIZE)
    {
        return 0;
    }
    addr = (unsigned int)(DL_BASE + (unsigned int)pos);
    for (i = 0; i < n; i++)
    {
        if (iap_code_write((unsigned int)(addr + i), buf[i]) == 0)
        {
            return 0;
        }
    }
    return 1;
}

unsigned char ota_finish(unsigned long file_len)
{
    unsigned int payload;
    unsigned int crc;
    unsigned int got;

    if ((s_busy == 0) || (s_erased == 0))
    {
        return 0;
    }
    if (iap_code_read(DL_BASE) != OADB_0)
    {
        return 0;
    }
    if (iap_code_read((unsigned int)(DL_BASE + 1U)) != OADB_1)
    {
        return 0;
    }
    if (iap_code_read((unsigned int)(DL_BASE + 2U)) != OADB_2)
    {
        return 0;
    }
    if (iap_code_read((unsigned int)(DL_BASE + 3U)) != OADB_3)
    {
        return 0;
    }
    payload = iap_code_read((unsigned int)(DL_BASE + 4U));
    payload |= (unsigned int)iap_code_read((unsigned int)(DL_BASE + 5U)) << 8;
    if ((iap_code_read((unsigned int)(DL_BASE + 6U)) != 0) ||
        (iap_code_read((unsigned int)(DL_BASE + 7U)) != 0))
    {
        return 0;
    }
    if ((payload == 0) || (payload > OTA_PAYLOAD_MAX))
    {
        return 0;
    }
    if (file_len != ((unsigned long)payload + OADB_HDR_N))
    {
        return 0;
    }
    crc = iap_code_read((unsigned int)(DL_BASE + 8U));
    crc |= (unsigned int)iap_code_read((unsigned int)(DL_BASE + 9U)) << 8;
    got = crc16_mem((unsigned int)(DL_BASE + OADB_HDR_N), payload);
    if (got != crc)
    {
        log_puts("OTA crc\r\n");
        return 0;
    }
    log_puts("OTA ok\r\n");
    write_magic();
    reboot_boot();
    return 1;
}

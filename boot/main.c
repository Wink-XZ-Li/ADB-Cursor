#include "SC95F876x_C.H"
#include "ota_map.h"
#include "boot_iap.h"

static unsigned int crc16(unsigned int addr, unsigned int n)
{
    unsigned int c;
    unsigned int i;
    unsigned char b;
    unsigned char k;

    c = 0xFFFF;
    for (i = 0; i < n; i++)
    {
        b = boot_code_read((unsigned int)(addr + i));
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
    }
    return c;
}

static unsigned char magic_ok(void)
{
    if (boot_ee_read(OTA_MAGIC_EE) != OTA_MAGIC0)
    {
        return 0;
    }
    if (boot_ee_read((unsigned int)(OTA_MAGIC_EE + 1U)) != OTA_MAGIC1)
    {
        return 0;
    }
    if (boot_ee_read((unsigned int)(OTA_MAGIC_EE + 2U)) != OTA_MAGIC2)
    {
        return 0;
    }
    if (boot_ee_read((unsigned int)(OTA_MAGIC_EE + 3U)) != OTA_MAGIC3)
    {
        return 0;
    }
    return 1;
}

static void magic_clear(void)
{
    boot_ee_erase(OTA_MAGIC_EE);
}

static void soft_reset(void)
{
    EA = 0;
    IAPADE = 0x00;
    ((void (code *)(void)) 0)();
}

static void jump_app(void)
{
    EA = 0;
    IAPADE = 0x00;
    ROMBNK = (unsigned char)((ROMBNK & 0xCF) | 0x10);
    ((void (code *)(void)) APP_ENTRY)();
}

static unsigned char copy_run(unsigned int payload)
{
    unsigned int dst;
    unsigned int src;
    unsigned int left;
    unsigned int n;
    unsigned int i;
    unsigned int k;
    unsigned char v;
    unsigned char ok;

    dst = RUN_BASE;
    while (dst < (unsigned int)(RUN_BASE + RUN_SIZE))
    {
        boot_code_erase(dst);
        dst = (unsigned int)(dst + IAP_SEC);
    }

    src = (unsigned int)(DL_BASE + OADB_HDR_N);
    dst = RUN_BASE;
    left = payload;
    while (left != 0)
    {
        n = left;
        if (n > IAP_CHUNK)
        {
            n = IAP_CHUNK;
        }
        ok = 0;
        for (i = 0; i < 2U; i++)
        {
            ok = 1;
            for (k = 0; k < n; k++)
            {
                v = boot_code_read((unsigned int)(src + k));
                boot_code_write((unsigned int)(dst + k), v);
                if (boot_code_read((unsigned int)(dst + k)) != v)
                {
                    ok = 0;
                    break;
                }
            }
            if (ok != 0)
            {
                break;
            }
        }
        if (ok == 0)
        {
            return 0;
        }
        src = (unsigned int)(src + n);
        dst = (unsigned int)(dst + n);
        left = (unsigned int)(left - n);
    }
    return 1;
}

static void apply_ota(void)
{
    unsigned int payload;
    unsigned int crc;
    unsigned int got;

    if (boot_code_read(DL_BASE) != OADB_0)
    {
        return;
    }
    if (boot_code_read((unsigned int)(DL_BASE + 1U)) != OADB_1)
    {
        return;
    }
    if (boot_code_read((unsigned int)(DL_BASE + 2U)) != OADB_2)
    {
        return;
    }
    if (boot_code_read((unsigned int)(DL_BASE + 3U)) != OADB_3)
    {
        return;
    }

    payload = boot_code_read((unsigned int)(DL_BASE + 4U));
    payload |= (unsigned int)boot_code_read((unsigned int)(DL_BASE + 5U)) << 8;
    if ((boot_code_read((unsigned int)(DL_BASE + 6U)) != 0) ||
        (boot_code_read((unsigned int)(DL_BASE + 7U)) != 0))
    {
        return;
    }
    if ((payload == 0) || (payload > OTA_PAYLOAD_MAX))
    {
        return;
    }
    crc = boot_code_read((unsigned int)(DL_BASE + 8U));
    crc |= (unsigned int)boot_code_read((unsigned int)(DL_BASE + 9U)) << 8;
    got = crc16((unsigned int)(DL_BASE + OADB_HDR_N), payload);
    if (got != crc)
    {
        return;
    }
    if (copy_run(payload) == 0)
    {
        return;
    }
    magic_clear();
}

void main(void)
{
    EA = 0;
    IAPADE = 0x00;
    WDTCON |= 0x10;
    if (magic_ok() != 0)
    {
        apply_ota();
        if (magic_ok() != 0)
        {
            soft_reset();
        }
    }
    jump_app();
    while (1)
    {
        WDTCON |= 0x10;
    }
}

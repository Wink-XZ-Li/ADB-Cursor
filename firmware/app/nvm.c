#include "board.h"
#include "nvm.h"
#include "eeprom.h"
#include "log_uart.h"

#define NVM_BASE     0x0000
#define NVM_MAGIC0   0xA5
#define NVM_MAGIC1   0x5A
#define NVM_VER      1U
#define NVM_N        12U
#define NVM_WAIT_MS  2000U

static unsigned char xdata s_img[8];
static unsigned char xdata s_now[8];
static unsigned char xdata s_buf[NVM_N];
static unsigned char xdata s_pend;
static unsigned int xdata s_wait;

static void rec_to_img(nvm_rec_t *rec, unsigned char *img)
{
    img[0] = rec->power;
    img[1] = rec->mode;
    img[2] = rec->fan;
    img[3] = rec->fan_saved;
    img[4] = rec->unit_f;
    img[5] = rec->sp;
    img[6] = rec->sleep;
    img[7] = rec->saver;
}

static unsigned char img_eq(unsigned char *a, unsigned char *b)
{
    unsigned char i;

    for (i = 0; i < 8U; i++)
    {
        if (a[i] != b[i])
        {
            return 0;
        }
    }
    return 1;
}

static unsigned char rec_ok(nvm_rec_t *rec)
{
    if (rec->power > 1U)
    {
        return 0;
    }
    if (rec->mode > 3U)
    {
        return 0;
    }
    if (rec->fan > 3U)
    {
        return 0;
    }
    if (rec->fan_saved > 3U)
    {
        return 0;
    }
    if (rec->unit_f > 1U)
    {
        return 0;
    }
    if (rec->sleep > 1U)
    {
        return 0;
    }
    if (rec->saver > 1U)
    {
        return 0;
    }
    if (rec->unit_f != 0)
    {
        if ((rec->sp < 61U) || (rec->sp > 88U))
        {
            return 0;
        }
    }
    else if ((rec->sp < 16U) || (rec->sp > 31U))
    {
        return 0;
    }
    return 1;
}

static unsigned char checksum(unsigned char *buf, unsigned char n)
{
    unsigned char s;
    unsigned char i;

    s = 0;
    for (i = 0; i < n; i++)
    {
        s = (unsigned char)(s + buf[i]);
    }
    return s;
}

static unsigned char do_write(unsigned char *img)
{
    unsigned char i;

    s_buf[0] = NVM_MAGIC0;
    s_buf[1] = NVM_MAGIC1;
    s_buf[2] = NVM_VER;
    for (i = 0; i < 8U; i++)
    {
        s_buf[3U + i] = img[i];
    }
    s_buf[11] = checksum(s_buf, 11);
    eeprom_erase_sector(NVM_BASE);
    for (i = 0; i < NVM_N; i++)
    {
        eeprom_write_byte((unsigned int)(NVM_BASE + i), s_buf[i]);
    }
    board_wdt_feed();
    for (i = 0; i < NVM_N; i++)
    {
        if (eeprom_read((unsigned int)(NVM_BASE + i)) != s_buf[i])
        {
            return 0;
        }
    }
    return 1;
}

unsigned char nvm_load(nvm_rec_t *rec)
{
    unsigned char i;

    s_pend = 0;
    s_wait = 0;
    for (i = 0; i < NVM_N; i++)
    {
        s_buf[i] = eeprom_read((unsigned int)(NVM_BASE + i));
    }
    if ((s_buf[0] != NVM_MAGIC0) || (s_buf[1] != NVM_MAGIC1) || (s_buf[2] != NVM_VER))
    {
        return 0;
    }
    if (s_buf[11] != checksum(s_buf, 11))
    {
        return 0;
    }
    rec->power = s_buf[3];
    rec->mode = s_buf[4];
    rec->fan = s_buf[5];
    rec->fan_saved = s_buf[6];
    rec->unit_f = s_buf[7];
    rec->sp = s_buf[8];
    rec->sleep = s_buf[9];
    rec->saver = s_buf[10];
    if (rec_ok(rec) == 0)
    {
        return 0;
    }
    return 1;
}

void nvm_capture(nvm_rec_t *rec)
{
    rec_to_img(rec, s_img);
    s_pend = 0;
    s_wait = 0;
}

void nvm_poll(nvm_rec_t *rec)
{
    rec_to_img(rec, s_now);
    if (img_eq(s_now, s_img) == 0)
    {
        rec_to_img(rec, s_img);
        s_pend = 1;
        s_wait = 0;
        return;
    }
    if (s_pend == 0)
    {
        return;
    }
    if (s_wait < NVM_WAIT_MS)
    {
        s_wait++;
        return;
    }
    s_pend = 0;
    if (do_write(s_img) != 0)
    {
        log_puts("MEM wr\r\n");
    }
    else
    {
        log_puts("MEM wr fail\r\n");
    }
}

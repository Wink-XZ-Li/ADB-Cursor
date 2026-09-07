#include "board.h"
#include "hmi.h"
#include "keys.h"
#include "disp_ui.h"
#include "log_uart.h"

#define MODE_COOL  0
#define MODE_DRY   1
#define MODE_HEAT  2
#define MODE_FAN   3

#define FAN_LOW    0
#define FAN_MED    1
#define FAN_HIGH   2
#define FAN_TURBO  3

static unsigned char xdata s_power;
static unsigned char xdata s_mode;
static unsigned char xdata s_fan;
static unsigned char xdata s_fan_saved;
static unsigned char xdata s_unit_f;
static unsigned char xdata s_sp;
static unsigned char xdata s_saver;
static unsigned char xdata s_edit;
static unsigned char xdata s_view;
static unsigned char xdata s_edit_val;
static unsigned char xdata s_timer_off;
static unsigned char xdata s_wifi_on;
static unsigned char xdata s_wifi_lamp;
static unsigned char xdata s_flash_on;
static unsigned char xdata s_suppress_pwr;
static unsigned char xdata s_dirty;
static unsigned int xdata s_idle_ms;
static unsigned int xdata s_edit_ms;
static unsigned int xdata s_view_ms;
static unsigned int xdata s_flash_ms;
static unsigned int xdata s_wifi_blink_ms;
static unsigned int xdata s_draw_ms;
static unsigned long xdata s_timer_ms;
static unsigned long xdata s_wifi_ms;
static unsigned char code s_fan_next[4] = {FAN_MED, FAN_HIGH, FAN_TURBO, FAN_LOW};
static unsigned char code s_mode_next[4] = {MODE_DRY, MODE_FAN, MODE_COOL, MODE_HEAT};

static unsigned char c_to_f(unsigned char c)
{
    unsigned int f;
    f = ((unsigned int)c * 9U) / 5U + 32U;
    if (f < 61U)
    {
        f = 61U;
    }
    if (f > 88U)
    {
        f = 88U;
    }
    return (unsigned char)f;
}

static unsigned char f_to_c(unsigned char f)
{
    unsigned int c;
    if (f < 32U)
    {
        return 16;
    }
    c = ((unsigned int)(f - 32U) * 5U) / 9U;
    if (c < 16U)
    {
        c = 16U;
    }
    if (c > 31U)
    {
        c = 31U;
    }
    return (unsigned char)c;
}

static void hmi_wake(void)
{
    s_idle_ms = 0;
    if (s_saver != 0)
    {
        s_saver = 0;
        s_dirty = 1;
    }
}

static void apply_dry_fan(void)
{
    if (s_mode == MODE_DRY)
    {
        s_fan = FAN_LOW;
    }
}

static void log_hmi_tag(char code *tag)
{
    log_puts(tag);
    log_puts("\r\n");
}

static void set_power(unsigned char on)
{
    s_power = on;
    s_edit = 0;
    s_view = 0;
    s_saver = 0;
    s_idle_ms = 0;
    if (on != 0)
    {
        apply_dry_fan();
        log_hmi_tag("HMI pwr=1");
    }
    else
    {
        log_hmi_tag("HMI pwr=0");
    }
    s_dirty = 1;
}

static void adj_temp(unsigned char up)
{
    if (s_unit_f != 0)
    {
        if ((up != 0) && (s_sp < 88U))
        {
            s_sp++;
        }
        if ((up == 0) && (s_sp > 61U))
        {
            s_sp--;
        }
    }
    else
    {
        if ((up != 0) && (s_sp < 31U))
        {
            s_sp++;
        }
        if ((up == 0) && (s_sp > 16U))
        {
            s_sp--;
        }
    }
    log_puts("HMI ts=");
    log_u16((unsigned int)s_sp);
    if (s_unit_f != 0)
    {
        log_puts("F\r\n");
    }
    else
    {
        log_puts("C\r\n");
    }
    s_dirty = 1;
}

static void adj_hours(unsigned char up)
{
    if ((up != 0) && (s_edit_val < 24U))
    {
        s_edit_val++;
    }
    if ((up == 0) && (s_edit_val > 0))
    {
        s_edit_val--;
    }
    s_edit_ms = 0;
    s_dirty = 1;
}

static void confirm_timer(void)
{
    if (s_edit_val == 0)
    {
        s_timer_ms = 0;
        log_hmi_tag("HMI tmr=0");
    }
    else
    {
        s_timer_ms = (unsigned long)s_edit_val * 3600000UL;
        s_timer_off = s_power;
        log_puts("HMI tmr=");
        log_u16((unsigned int)s_edit_val);
        log_puts("h\r\n");
    }
    s_edit = 0;
    s_view = 0;
    s_dirty = 1;
}

static void handle_power(unsigned char evt)
{
    if (evt == KEY_EVT_LONG)
    {
        if (s_power == 0)
        {
            s_wifi_on = 1;
            s_wifi_ms = 180000UL;
            s_wifi_blink_ms = 0;
            s_wifi_lamp = 1;
            s_suppress_pwr = 1;
            log_hmi_tag("HMI wifi");
            s_dirty = 1;
        }
        return;
    }
    if (evt != KEY_EVT_CLICK)
    {
        return;
    }
    if (s_suppress_pwr != 0)
    {
        s_suppress_pwr = 0;
        return;
    }
    if (s_power != 0)
    {
        s_timer_ms = 0;
        set_power(0);
    }
    else
    {
        s_timer_ms = 0;
        set_power(1);
    }
}

static void handle_fan(unsigned char evt)
{
    if (evt == KEY_EVT_LONG)
    {
        if (s_power == 0)
        {
            if (s_unit_f != 0)
            {
                s_sp = f_to_c(s_sp);
                s_unit_f = 0;
                log_hmi_tag("HMI unit=C");
            }
            else
            {
                s_sp = c_to_f(s_sp);
                s_unit_f = 1;
                log_hmi_tag("HMI unit=F");
            }
            s_dirty = 1;
        }
        return;
    }
    if ((evt != KEY_EVT_CLICK) || (s_power == 0) || (s_mode == MODE_DRY))
    {
        return;
    }
    s_fan = s_fan_next[s_fan];
    s_fan_saved = s_fan;
    log_puts("HMI fan=");
    log_u16((unsigned int)s_fan);
    log_puts("\r\n");
    s_dirty = 1;
}

static void handle_mode(void)
{
    unsigned char next;

    if (s_power == 0)
    {
        return;
    }
    next = s_mode_next[s_mode];
    if (s_mode == MODE_DRY)
    {
        s_fan = s_fan_saved;
    }
    if (next == MODE_DRY)
    {
        s_fan_saved = s_fan;
        s_fan = FAN_LOW;
    }
    s_mode = next;
    log_puts("HMI mode=");
    log_u16((unsigned int)s_mode);
    log_puts("\r\n");
    s_dirty = 1;
}

static void handle_timer(void)
{
    if (s_edit != 0)
    {
        confirm_timer();
        return;
    }
    if (s_view != 0)
    {
        s_timer_ms = 0;
        s_view = 0;
        log_hmi_tag("HMI tmr=0");
        s_dirty = 1;
        return;
    }
    if (s_timer_ms != 0)
    {
        s_view = 1;
        s_view_ms = 0;
        s_dirty = 1;
        return;
    }
    s_edit = 1;
    s_edit_val = 1;
    s_edit_ms = 0;
    s_flash_ms = 0;
    s_flash_on = 1;
    log_hmi_tag("HMI tmr edit");
    s_dirty = 1;
}

static void handle_adj(unsigned char up)
{
    if (s_edit != 0)
    {
        adj_hours(up);
        return;
    }
    if (s_power == 0)
    {
        return;
    }
    adj_temp(up);
}

static void hmi_draw(void)
{
    unsigned char show_num;
    unsigned char num;
    unsigned char timer_lamp;
    unsigned char hours;

    show_num = 0;
    num = 0;
    timer_lamp = 0;
    if (s_timer_ms != 0)
    {
        timer_lamp = 1;
    }

    if (s_edit != 0)
    {
        timer_lamp = 1;
        if (s_flash_on != 0)
        {
            show_num = 1;
            num = s_edit_val;
        }
    }
    else if (s_view != 0)
    {
        timer_lamp = 1;
        hours = (unsigned char)((s_timer_ms + 3599999UL) / 3600000UL);
        if (hours == 0)
        {
            hours = 1;
        }
        show_num = 1;
        num = hours;
    }
    else if (s_power != 0)
    {
        show_num = 1;
        num = s_sp;
    }

    disp_ui_draw(
        s_power,
        s_saver,
        s_mode,
        s_fan,
        show_num,
        num,
        timer_lamp,
        s_wifi_lamp,
        s_saver);
}

void hmi_init(void)
{
    s_power = 0;
    s_mode = MODE_COOL;
    s_fan = FAN_MED;
    s_fan_saved = FAN_MED;
    s_unit_f = 1;
    s_sp = 72;
    s_saver = 0;
    s_edit = 0;
    s_view = 0;
    s_timer_ms = 0;
    s_timer_off = 0;
    s_wifi_on = 1;
    s_wifi_ms = 180000UL;
    s_wifi_lamp = 1;
    s_wifi_blink_ms = 0;
    s_flash_on = 1;
    s_flash_ms = 0;
    s_suppress_pwr = 0;
    s_idle_ms = 0;
    s_edit_ms = 0;
    s_view_ms = 0;
    s_draw_ms = 0;
    s_dirty = 1;
    hmi_draw();
}

void hmi_log_status(void)
{
    log_puts("HMI p=");
    log_u16((unsigned int)s_power);
    log_puts(" m=");
    log_u16((unsigned int)s_mode);
    log_puts(" f=");
    log_u16((unsigned int)s_fan);
    log_puts(" ts=");
    log_u16((unsigned int)s_sp);
    if (s_unit_f != 0)
    {
        log_puts("F");
    }
    else
    {
        log_puts("C");
    }
    log_puts(" tmr=");
    log_u16((unsigned int)(s_timer_ms / 3600000UL));
    log_puts(" w=");
    log_u16((unsigned int)s_wifi_on);
    log_puts(" sav=");
    log_u16((unsigned int)s_saver);
    log_puts("\r\n");
}

void hmi_poll(void)
{
    unsigned char i;
    unsigned char e;

    for (i = 0; i < KEY_N; i++)
    {
        e = keys_take_evt(i);
        if (e == KEY_EVT_NONE)
        {
            continue;
        }
        hmi_wake();
        if (i == KEY_POWER)
        {
            handle_power(e);
        }
        else if (i == KEY_FAN)
        {
            handle_fan(e);
        }
        else if (i == KEY_MODE)
        {
            handle_mode();
        }
        else if (i == KEY_TIMER)
        {
            handle_timer();
        }
        else if (i == KEY_UP)
        {
            handle_adj(1);
        }
        else
        {
            handle_adj(0);
        }
    }

    if ((s_power != 0) && (s_edit == 0) && (s_view == 0) && (s_saver == 0))
    {
        if (s_idle_ms < 15000U)
        {
            s_idle_ms++;
        }
        else
        {
            s_saver = 1;
            s_dirty = 1;
            log_hmi_tag("HMI saver=1");
        }
    }

    if (s_edit != 0)
    {
        s_edit_ms++;
        s_flash_ms++;
        if (s_flash_ms >= 500U)
        {
            s_flash_ms = 0;
            s_flash_on = (unsigned char)(s_flash_on == 0);
            s_dirty = 1;
        }
        if (s_edit_ms >= 5000U)
        {
            confirm_timer();
        }
    }

    if (s_view != 0)
    {
        s_view_ms++;
        if (s_view_ms >= 5000U)
        {
            s_view = 0;
            s_dirty = 1;
        }
    }

    if (s_timer_ms != 0)
    {
        s_timer_ms--;
        if (s_timer_ms == 0)
        {
            if (s_timer_off != 0)
            {
                set_power(0);
            }
            else
            {
                set_power(1);
            }
            log_hmi_tag("HMI tmr done");
        }
    }

    if (s_wifi_on != 0)
    {
        if (s_wifi_ms != 0)
        {
            s_wifi_ms--;
        }
        s_wifi_blink_ms++;
        if (s_wifi_blink_ms >= 250U)
        {
            s_wifi_blink_ms = 0;
            s_wifi_lamp = (unsigned char)(s_wifi_lamp == 0);
            s_dirty = 1;
        }
        if (s_wifi_ms == 0)
        {
            s_wifi_on = 0;
            s_wifi_lamp = 0;
            s_dirty = 1;
            log_hmi_tag("HMI wifi=0");
        }
    }

    s_draw_ms++;
    if ((s_dirty != 0) || (s_draw_ms >= 50U))
    {
        s_draw_ms = 0;
        s_dirty = 0;
        hmi_draw();
    }
}

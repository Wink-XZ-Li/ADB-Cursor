#include "board.h"
#include "hmi.h"
#include "keys.h"
#include "disp_ui.h"
#include "log_uart.h"
#include "ntc.h"
#include "tuya_link.h"
#include "nvm.h"

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
static unsigned char xdata s_wifi_lamp;
static unsigned char xdata s_wifi_st;
static unsigned char xdata s_flash_on;
static unsigned char xdata s_suppress_pwr;
static unsigned char xdata s_dirty;
static unsigned char xdata s_pwr_lost;
static unsigned char xdata s_pwr_fault;
static unsigned char xdata s_last_amb;
static unsigned char xdata s_sleep;
static unsigned char xdata s_on_min;
static unsigned char xdata s_wifi_rpt;
static unsigned int xdata s_show_set_ms;
static unsigned int xdata s_edit_ms;
static unsigned int xdata s_view_ms;
static unsigned int xdata s_flash_ms;
static unsigned int xdata s_wifi_blink_ms;
static unsigned int xdata s_draw_ms;
static unsigned long xdata s_timer_ms;
static unsigned long xdata s_on_ms;
static unsigned long xdata s_usage_ms;
static nvm_rec_t xdata s_nvm;
static unsigned char code s_fan_next[4] = {FAN_MED, FAN_HIGH, FAN_TURBO, FAN_LOW};
static unsigned char code s_mode_next[4] = {MODE_DRY, MODE_FAN, MODE_COOL, MODE_HEAT};

static unsigned char c_to_f(unsigned char c)
{
    unsigned int f;
    f = ((unsigned int)c * 9U + 2U) / 5U + 32U;
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

static void peek_setpoint(void)
{
    s_show_set_ms = 2000U;
    s_dirty = 1;
}

static unsigned char amb_display(void)
{
    unsigned int f;

    if (s_unit_f == 0)
    {
        if (ntc_c() > 99U)
        {
            return 99;
        }
        return ntc_c();
    }
    f = ((unsigned int)ntc_c() * 9U + 2U) / 5U + 32U;
    if (f > 99U)
    {
        f = 99U;
    }
    return (unsigned char)f;
}

static void mark_rpt(unsigned char bits)
{
    s_wifi_rpt = (unsigned char)(s_wifi_rpt | bits);
}

static void reset_usage(void)
{
    s_on_ms = 0;
    s_on_min = 0;
    s_usage_ms = 0;
    mark_rpt(HMI_RPT_USAGE);
}

static void hmi_wake(void)
{
    if (s_saver != 0)
    {
        s_saver = 0;
        s_dirty = 1;
        mark_rpt(HMI_RPT_FULL);
        log_puts("HMI scr=0\r\n");
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
    s_show_set_ms = 0;
    reset_usage();
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
    mark_rpt(HMI_RPT_FULL);
}

static void adj_temp(unsigned char up)
{
    unsigned char old;

    old = s_sp;
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
    if (s_sp == old)
    {
        return;
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
    peek_setpoint();
    mark_rpt(HMI_RPT_FULL);
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
    mark_rpt(HMI_RPT_FULL);
}

static void handle_power(unsigned char evt)
{
    if (evt == KEY_EVT_LONG)
    {
        if (s_power == 0)
        {
            tuya_link_reset_wifi();
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
            mark_rpt(HMI_RPT_FULL);
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
    mark_rpt(HMI_RPT_FULL);
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
    mark_rpt(HMI_RPT_FULL);
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
    unsigned char overlay;
    unsigned char saver_draw;
    unsigned char ov_scr;

    show_num = 0;
    num = 0;
    timer_lamp = 0;
    overlay = DISP_OV_NONE;
    ov_scr = 0;
    if (s_timer_ms != 0)
    {
        timer_lamp = 1;
    }

    if ((s_power != 0) && (s_pwr_lost != 0))
    {
        overlay = DISP_OV_DASH;
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
        if (s_pwr_lost == 0)
        {
            if (s_pwr_fault == 6U)
            {
                overlay = DISP_OV_E1;
            }
            else if (s_pwr_fault == 7U)
            {
                overlay = DISP_OV_E2;
            }
        }
        if ((overlay == DISP_OV_E1) || (overlay == DISP_OV_E2))
        {
            show_num = 0;
        }
        else if (s_show_set_ms != 0)
        {
            show_num = 1;
            num = s_sp;
        }
        else if (ntc_ready() != 0)
        {
            show_num = 1;
            num = amb_display();
        }
        else
        {
            show_num = 0;
        }
    }

    if ((s_edit != 0) || (s_view != 0))
    {
        ov_scr = 1;
    }
    if ((overlay == DISP_OV_E1) || (overlay == DISP_OV_E2) || (overlay == DISP_OV_DASH))
    {
        ov_scr = 1;
    }
    saver_draw = s_saver;
    if (ov_scr != 0)
    {
        saver_draw = 0;
    }
    else if (s_saver != 0)
    {
        show_num = 0;
    }

    disp_ui_draw(
        s_power,
        saver_draw,
        s_mode,
        s_fan,
        show_num,
        num,
        timer_lamp,
        s_wifi_lamp,
        saver_draw,
        overlay);
}

static void fill_nvm(void)
{
    s_nvm.power = (unsigned char)((s_power != 0) ? 1 : 0);
    s_nvm.mode = s_mode;
    s_nvm.fan = s_fan;
    s_nvm.fan_saved = s_fan_saved;
    s_nvm.unit_f = (unsigned char)((s_unit_f != 0) ? 1 : 0);
    s_nvm.sp = s_sp;
    s_nvm.sleep = (unsigned char)((s_sleep != 0) ? 1 : 0);
    s_nvm.saver = (unsigned char)((s_saver != 0) ? 1 : 0);
}

static void apply_nvm(void)
{
    if (nvm_load(&s_nvm) == 0)
    {
        log_puts("MEM miss\r\n");
        fill_nvm();
        nvm_capture(&s_nvm);
        return;
    }
    s_power = s_nvm.power;
    s_mode = s_nvm.mode;
    s_fan = s_nvm.fan;
    s_fan_saved = s_nvm.fan_saved;
    s_unit_f = s_nvm.unit_f;
    s_sp = s_nvm.sp;
    s_sleep = s_nvm.sleep;
    s_saver = s_nvm.saver;
    apply_dry_fan();
    mark_rpt(HMI_RPT_FULL);
    log_puts("MEM ok p=");
    log_u16((unsigned int)s_power);
    log_puts(" m=");
    log_u16((unsigned int)s_mode);
    log_puts(" f=");
    log_u16((unsigned int)s_fan);
    log_puts(" ts=");
    log_u16((unsigned int)s_sp);
    log_puts("\r\n");
    fill_nvm();
    nvm_capture(&s_nvm);
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
    s_wifi_lamp = 0;
    s_wifi_blink_ms = 0;
    s_wifi_st = 0xFF;
    s_flash_on = 1;
    s_flash_ms = 0;
    s_suppress_pwr = 0;
    s_edit_ms = 0;
    s_view_ms = 0;
    s_draw_ms = 0;
    s_dirty = 1;
    s_pwr_lost = 0;
    s_pwr_fault = 0;
    s_show_set_ms = 0;
    s_last_amb = 0xFF;
    s_sleep = 0;
    s_on_ms = 0;
    s_on_min = 0;
    s_usage_ms = 0;
    s_wifi_rpt = 0;
    apply_nvm();
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
    log_u16((unsigned int)s_wifi_st);
    log_puts(" sav=");
    log_u16((unsigned int)s_saver);
    log_puts(" slp=");
    log_u16((unsigned int)s_sleep);
    log_puts(" use=");
    log_u16((unsigned int)s_on_min);
    log_puts(" lost=");
    log_u16((unsigned int)s_pwr_lost);
    log_puts(" flt=");
    log_u16((unsigned int)s_pwr_fault);
    log_puts("\r\n");
}

unsigned char hmi_power(void)
{
    return s_power;
}

unsigned char hmi_mode(void)
{
    return s_mode;
}

unsigned char hmi_fan(void)
{
    return s_fan;
}

unsigned char hmi_setpoint_c(void)
{
    if (s_unit_f != 0)
    {
        return f_to_c(s_sp);
    }
    return s_sp;
}

unsigned char hmi_setpoint_disp(void)
{
    return s_sp;
}

unsigned char hmi_unit_f(void)
{
    return s_unit_f;
}

unsigned char hmi_mode_tuya(void)
{
    if (s_mode == MODE_COOL)
    {
        return 2;
    }
    if (s_mode == MODE_DRY)
    {
        return 0;
    }
    if (s_mode == MODE_HEAT)
    {
        return 3;
    }
    return 1;
}

unsigned char hmi_sleep(void)
{
    return s_sleep;
}

unsigned char hmi_saver(void)
{
    return s_saver;
}

unsigned char hmi_usage_min(void)
{
    return s_on_min;
}

unsigned char hmi_fault_bits(void)
{
    if (s_pwr_fault == 6U)
    {
        return 0x01;
    }
    if (s_pwr_fault == 7U)
    {
        return 0x02;
    }
    return 0;
}

unsigned char hmi_wifi_take_rpt(void)
{
    unsigned char r;

    r = s_wifi_rpt;
    s_wifi_rpt = 0;
    return r;
}

void hmi_wifi_set_power(unsigned char on)
{
    if (on != s_power)
    {
        set_power((unsigned char)((on != 0) ? 1 : 0));
    }
}

void hmi_wifi_set_mode_tuya(unsigned char tuya_mode)
{
    unsigned char mode;

    if (tuya_mode == 0)
    {
        mode = MODE_DRY;
    }
    else if (tuya_mode == 1)
    {
        mode = MODE_FAN;
    }
    else if (tuya_mode == 2)
    {
        mode = MODE_COOL;
    }
    else if (tuya_mode == 3)
    {
        mode = MODE_HEAT;
    }
    else
    {
        return;
    }
    if (mode == s_mode)
    {
        return;
    }
    if (s_mode == MODE_DRY)
    {
        s_fan = s_fan_saved;
    }
    if (mode == MODE_DRY)
    {
        s_fan_saved = s_fan;
        s_fan = FAN_LOW;
    }
    s_mode = mode;
    apply_dry_fan();
    log_puts("HMI mode=");
    log_u16((unsigned int)s_mode);
    log_puts("\r\n");
    s_dirty = 1;
}

void hmi_wifi_set_fan(unsigned char fan)
{
    if (fan > FAN_TURBO)
    {
        return;
    }
    if ((s_mode != MODE_DRY) && (fan != s_fan))
    {
        s_fan = fan;
        s_fan_saved = fan;
        log_puts("HMI fan=");
        log_u16((unsigned int)s_fan);
        log_puts("\r\n");
        s_dirty = 1;
    }
    apply_dry_fan();
}

void hmi_wifi_set_temp_c(unsigned char c)
{
    unsigned char sp;

    if (c < 16U)
    {
        c = 16;
    }
    if (c > 31U)
    {
        c = 31;
    }
    if (s_unit_f != 0)
    {
        sp = c_to_f(c);
    }
    else
    {
        sp = c;
    }
    if (sp != s_sp)
    {
        s_sp = sp;
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
        peek_setpoint();
        mark_rpt(HMI_RPT_FULL);
    }
}

void hmi_wifi_set_temp_f(unsigned char f)
{
    unsigned char sp;

    if (f < 61U)
    {
        f = 61;
    }
    if (f > 88U)
    {
        f = 88;
    }
    if (s_unit_f != 0)
    {
        sp = f;
    }
    else
    {
        sp = f_to_c(f);
    }
    if (sp != s_sp)
    {
        s_sp = sp;
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
        peek_setpoint();
        mark_rpt(HMI_RPT_FULL);
    }
}

void hmi_wifi_set_unit(unsigned char unit_f)
{
    if (unit_f == s_unit_f)
    {
        return;
    }
    if (unit_f != 0)
    {
        s_sp = c_to_f(s_sp);
        s_unit_f = 1;
        log_hmi_tag("HMI unit=F");
    }
    else
    {
        s_sp = f_to_c(s_sp);
        s_unit_f = 0;
        log_hmi_tag("HMI unit=C");
    }
    s_dirty = 1;
    mark_rpt(HMI_RPT_FULL);
}

void hmi_wifi_set_sleep(unsigned char on)
{
    on = (unsigned char)((on != 0) ? 1 : 0);
    if (on != s_sleep)
    {
        s_sleep = on;
        log_puts("HMI slp=");
        log_u16((unsigned int)s_sleep);
        log_puts("\r\n");
    }
}

void hmi_wifi_set_saver(unsigned char on)
{
    on = (unsigned char)((on != 0) ? 1 : 0);
    if (on != s_saver)
    {
        s_saver = on;
        s_dirty = 1;
        log_puts("HMI scr=");
        log_u16((unsigned int)s_saver);
        log_puts("\r\n");
    }
}

void hmi_set_pwr_lost(unsigned char lost)
{
    if (s_pwr_lost != lost)
    {
        s_pwr_lost = lost;
        s_dirty = 1;
    }
}

void hmi_set_pwr_fault(unsigned char fault)
{
    if (s_pwr_fault != fault)
    {
        s_pwr_fault = fault;
        s_dirty = 1;
        mark_rpt(HMI_RPT_FULL);
    }
}

void hmi_apply_ir(unsigned char power, unsigned char mode_ok, unsigned char mode,
                  unsigned char fan, unsigned char set_c, unsigned char f_plus,
                  unsigned char unit_f, unsigned char tmr_op, unsigned char tmr_hours,
                  unsigned char sleep, unsigned char disp_on)
{
    unsigned char sp;
    unsigned char was_on;
    unsigned char old_sp;
    unsigned char old_unit;
    unsigned char saver;

    s_edit = 0;
    s_view = 0;
    was_on = s_power;
    old_sp = s_sp;
    old_unit = s_unit_f;

    if (unit_f != s_unit_f)
    {
        s_unit_f = unit_f;
        if (s_unit_f != 0)
        {
            log_hmi_tag("HMI unit=F");
        }
        else
        {
            log_hmi_tag("HMI unit=C");
        }
        s_dirty = 1;
    }

    if (s_unit_f != 0)
    {
        sp = c_to_f(set_c);
        if (f_plus != 0)
        {
            if (sp < 88U)
            {
                sp++;
            }
        }
    }
    else
    {
        sp = set_c;
        if (sp < 16U)
        {
            sp = 16;
        }
        if (sp > 31U)
        {
            sp = 31;
        }
    }
    if (sp != s_sp)
    {
        s_sp = sp;
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

    if ((was_on != 0) && ((s_sp != old_sp) || (s_unit_f != old_unit)))
    {
        peek_setpoint();
    }

    if ((mode_ok != 0) && (mode != s_mode))
    {
        if (s_mode == MODE_DRY)
        {
            s_fan = s_fan_saved;
        }
        if (mode == MODE_DRY)
        {
            s_fan_saved = s_fan;
            s_fan = FAN_LOW;
        }
        s_mode = mode;
        log_puts("HMI mode=");
        log_u16((unsigned int)s_mode);
        log_puts("\r\n");
        s_dirty = 1;
    }

    if ((fan <= FAN_HIGH) && (s_mode != MODE_DRY))
    {
        if (fan != s_fan)
        {
            s_fan = fan;
            s_fan_saved = fan;
            log_puts("HMI fan=");
            log_u16((unsigned int)s_fan);
            log_puts("\r\n");
            s_dirty = 1;
        }
    }
    apply_dry_fan();

    if (tmr_op == 3U)
    {
        if (s_timer_ms != 0)
        {
            s_timer_ms = 0;
            log_hmi_tag("HMI tmr=0");
            s_dirty = 1;
        }
    }
    else if ((tmr_op == 1U) || (tmr_op == 2U))
    {
        s_timer_ms = (unsigned long)tmr_hours * 3600000UL;
        s_timer_off = (unsigned char)((tmr_op == 1U) ? 1 : 0);
        log_puts("HMI tmr=");
        log_u16((unsigned int)tmr_hours);
        log_puts("h\r\n");
        s_dirty = 1;
    }

    if (power != s_power)
    {
        set_power(power);
    }

    sleep = (unsigned char)((sleep != 0) ? 1 : 0);
    if (sleep != s_sleep)
    {
        s_sleep = sleep;
        log_puts("HMI slp=");
        log_u16((unsigned int)s_sleep);
        log_puts("\r\n");
        s_dirty = 1;
    }

    saver = (unsigned char)((disp_on != 0) ? 0 : 1);
    if (saver != s_saver)
    {
        s_saver = saver;
        log_puts("HMI scr=");
        log_u16((unsigned int)s_saver);
        log_puts("\r\n");
        s_dirty = 1;
    }
    mark_rpt(HMI_RPT_FULL);
}

void hmi_poll(void)
{
    unsigned char i;
    unsigned char e;
    unsigned char st;
    unsigned int blink;

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

    if (s_show_set_ms != 0)
    {
        s_show_set_ms--;
        if (s_show_set_ms == 0)
        {
            s_dirty = 1;
        }
    }

    if (ntc_ready() != 0)
    {
        if (ntc_c() != s_last_amb)
        {
            s_last_amb = ntc_c();
            s_dirty = 1;
            mark_rpt(HMI_RPT_TEMP);
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

    if (s_power != 0)
    {
        s_on_ms++;
        if (s_on_ms >= 60000UL)
        {
            s_on_ms = 0;
            if (s_on_min < 30U)
            {
                s_on_min++;
            }
        }
        s_usage_ms++;
        if (s_usage_ms >= 1800000UL)
        {
            s_usage_ms = 0;
            mark_rpt(HMI_RPT_USAGE);
            log_puts("HMI use=");
            log_u16((unsigned int)s_on_min);
            log_puts("\r\n");
        }
    }

    st = tuya_link_wifi_state();
        if (st != s_wifi_st)
        {
            s_wifi_st = st;
            s_wifi_blink_ms = 0;
            s_dirty = 1;
        }
        if ((st == 0) || (st == 6U))
        {
            blink = 250U;
        }
        else if (st == 1U)
        {
            blink = 1500U;
        }
        else
        {
            blink = 0;
        }
        if (blink != 0)
        {
            s_wifi_blink_ms++;
            if (s_wifi_blink_ms >= blink)
            {
                s_wifi_blink_ms = 0;
                s_wifi_lamp = (unsigned char)(s_wifi_lamp == 0);
                s_dirty = 1;
            }
        }
        else if ((st == 3U) || (st == 4U))
        {
            if (s_wifi_lamp == 0)
            {
                s_wifi_lamp = 1;
                s_dirty = 1;
            }
        }
    else if (s_wifi_lamp != 0)
    {
        s_wifi_lamp = 0;
        s_dirty = 1;
    }

    s_draw_ms++;
    if ((s_dirty != 0) || (s_draw_ms >= 50U))
    {
        s_draw_ms = 0;
        s_dirty = 0;
        hmi_draw();
    }

    fill_nvm();
    nvm_poll(&s_nvm);
}

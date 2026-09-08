#ifndef HMI_H
#define HMI_H

#define HMI_RPT_FULL   0x01
#define HMI_RPT_USAGE  0x02
#define HMI_RPT_TEMP   0x04

void hmi_init(void);
void hmi_poll(void);
void hmi_log_status(void);
unsigned char hmi_power(void);
unsigned char hmi_mode(void);
unsigned char hmi_mode_tuya(void);
unsigned char hmi_fan(void);
unsigned char hmi_setpoint_c(void);
unsigned char hmi_setpoint_disp(void);
unsigned char hmi_unit_f(void);
unsigned char hmi_sleep(void);
unsigned char hmi_saver(void);
unsigned char hmi_usage_min(void);
unsigned char hmi_fault_bits(void);
unsigned char hmi_wifi_take_rpt(void);
void hmi_wifi_set_power(unsigned char on);
void hmi_wifi_set_mode_tuya(unsigned char tuya_mode);
void hmi_wifi_set_fan(unsigned char fan);
void hmi_wifi_set_temp_c(unsigned char c);
void hmi_wifi_set_temp_f(unsigned char f);
void hmi_wifi_set_unit(unsigned char unit_f);
void hmi_wifi_set_sleep(unsigned char on);
void hmi_wifi_set_saver(unsigned char on);
void hmi_set_pwr_lost(unsigned char lost);
void hmi_set_pwr_fault(unsigned char fault);
void hmi_set_ota(unsigned char on);
unsigned char hmi_ota_busy(void);
void hmi_apply_ir(unsigned char power, unsigned char mode_ok, unsigned char mode,
                  unsigned char fan, unsigned char set_c, unsigned char f_plus,
                  unsigned char unit_f, unsigned char tmr_op, unsigned char tmr_hours,
                  unsigned char sleep, unsigned char disp_on);

#endif

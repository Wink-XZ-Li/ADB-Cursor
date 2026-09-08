#ifndef HMI_H
#define HMI_H

void hmi_init(void);
void hmi_poll(void);
void hmi_log_status(void);
unsigned char hmi_power(void);
unsigned char hmi_mode(void);
unsigned char hmi_fan(void);
unsigned char hmi_setpoint_c(void);
void hmi_set_pwr_lost(unsigned char lost);
void hmi_set_pwr_fault(unsigned char fault);
void hmi_apply_ir(unsigned char power, unsigned char mode_ok, unsigned char mode,
                  unsigned char fan, unsigned char set_c, unsigned char f_plus,
                  unsigned char unit_f, unsigned char tmr_op, unsigned char tmr_hours);

#endif

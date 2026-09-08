#ifndef TUYA_LINK_H
#define TUYA_LINK_H

void tuya_link_init(void);
void tuya_link_poll(void);
void tuya_link_reset_wifi(void);
unsigned char tuya_link_wifi_state(void);

#endif

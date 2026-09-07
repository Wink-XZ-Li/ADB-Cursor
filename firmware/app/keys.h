#ifndef KEYS_H
#define KEYS_H

#define KEY_POWER  0
#define KEY_FAN    1
#define KEY_TIMER  2
#define KEY_MODE   3
#define KEY_UP     4
#define KEY_DOWN   5
#define KEY_N      6

#define KEY_EVT_NONE    0
#define KEY_EVT_CLICK   1
#define KEY_EVT_LONG    2
#define KEY_EVT_REPEAT  3

void keys_init(void);
void keys_poll(void);
unsigned char keys_take_evt(unsigned char id);

#endif

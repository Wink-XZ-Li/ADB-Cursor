#ifndef TM1640_H
#define TM1640_H

#define TM1640_GRID_COUNT  16

void tm1640_init(void);
void tm1640_display(unsigned char *buf);
void tm1640_blank(void);

#endif

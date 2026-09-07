#ifndef TM1640_H
#define TM1640_H

#define TM1640_GRID_COUNT  16

#define TM1640_BR_FULL  0x8F
#define TM1640_BR_DIM   0x88

void tm1640_init(void);
void tm1640_display(unsigned char *buf);
void tm1640_display_br(unsigned char *buf, unsigned char br);
void tm1640_blank(void);

#endif

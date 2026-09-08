#ifndef IR_RX_H
#define IR_RX_H

#define IR_FRAME_N  15

void ir_rx_init(void);
unsigned char ir_rx_take(unsigned char *dst);

#endif

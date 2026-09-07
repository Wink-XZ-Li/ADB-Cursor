#ifndef PWR_UART_H
#define PWR_UART_H

void pwr_uart_init(void);
void pwr_uart_putc(unsigned char c);
void pwr_uart_poll_rx(void);
unsigned char pwr_uart_getc(unsigned char *out);

#endif

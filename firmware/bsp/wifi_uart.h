#ifndef WIFI_UART_H
#define WIFI_UART_H

void wifi_uart_power(void);
void wifi_uart_init(void);
void wifi_uart_pins(void);
void wifi_uart_poll_rx(void);
unsigned char wifi_uart_putc(unsigned char c);
unsigned char wifi_uart_rx_take(unsigned char *c);

#endif

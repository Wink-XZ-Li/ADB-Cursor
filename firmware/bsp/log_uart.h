#ifndef LOG_UART_H
#define LOG_UART_H

void log_uart_init(void);
void log_putc(unsigned char c);
void log_puts(char code *s);
void log_u16(unsigned int v);
void log_hex8(unsigned char v);
void log_banner(unsigned int tick);

#endif

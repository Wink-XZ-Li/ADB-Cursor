#ifndef BOARD_H
#define BOARD_H

#include "SC95F876x_C.H"
#include "intrins.h"
#include "version.h"

/*
 * Clock is taken from the Keil device CLOCK(32000000) field and the official
 * SC95F876x datasheet internal IRC options (32/16/8/4 MHz). Code Option is
 * not modified. If Option is not 32 MHz, log baud will be wrong.
 */
#define SYSCLK_HZ           32000000UL
#define SYSCLK_MHZ          32U

#define LOG_UART_BAUD       115200UL
#define LOG_BAUD_DIV        ((unsigned int)(SYSCLK_HZ / LOG_UART_BAUD))

#define PWR_UART_BAUD       4800UL
#define PWR_BAUD_DIV        ((unsigned int)(SYSCLK_HZ / PWR_UART_BAUD))

/*
 * DispUart: user constraint + datasheet USCI0 USTX0/USRX0 on P0.5/P0.6.
 * Power-board UART: P4.4 TX / P4.5 RX, USCI2, 4800 8N1.
 * WiFi UART P2.1/P2.0 is not initialized in this stage.
 */
#define LOG_TX_PIN          P05
#define LOG_RX_PIN          P06
#define PWR_TX_PIN          P44
#define PWR_RX_PIN          P45

void board_init(void);
void board_wdt_feed(void);

#endif

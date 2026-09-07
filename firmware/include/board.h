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

/*
 * DispUart: user constraint + datasheet USCI0 USTX0/USRX0 on P0.5/P0.6.
 * Power-board UART and WiFi UART are not used as log in this stage.
 */
#define LOG_TX_PIN          P05
#define LOG_RX_PIN          P06

void board_init(void);
void board_wdt_feed(void);

#endif

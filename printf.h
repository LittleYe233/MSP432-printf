#ifndef PRINTF_H_
#define PRINTF_H_

#include <stdint.h>

#define MY_PRINTF_UART_BASE EUSCI_A0_BASE

void uart_init(void);
void myprintf(const char *, ...);

#endif /* PRINTF_H_ */

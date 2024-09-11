/*
 * printf library for the MSP432
 *
 * Largely taken from and inspired from:
 * 	http://www.msp430launchpad.com/2012/06/using-printf.html
 *	http://www.43oh.com/forum/viewtopic.php?f=10&t=1732
 *
 * See http://www.samlewis.me for an example implementation.
 */

#include "printf.h"
#include "driverlib.h"
#include "stdarg.h"
#include <stdint.h>

/// @note Baud rate = 9600
const eUSCI_UART_Config uartConfig = {
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source
    78,                                           // BRDIV = 78
    2,                                            // UCxBRF = 2
    0,                                            // UCxBRS = 0
    EUSCI_A_UART_NO_PARITY,                       // No Parity
    EUSCI_A_UART_LSB_FIRST,                       // MSB First
    EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
    EUSCI_A_UART_MODE,                            // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
};

void sendByte(uint32_t moduleInstance, char c) {
  MAP_UART_transmitData(moduleInstance, c);
}

static const unsigned long dv[] = {
    1000000000, // +0
    100000000,  // +1
    10000000,   // +2
    1000000,    // +3
    100000,     // +4
    10000,      // +5
    1000,       // +6
    100,        // +7
    10,         // +8
    1,          // +9
};

void myputs(uint32_t moduleInstance, char *s) {
  char c;

  while ((c = *s++)) {
    sendByte(moduleInstance, c);
  }
}

void myputc(uint32_t moduleInstance, unsigned b) { sendByte(moduleInstance, b); }

static void xtoa(uint32_t moduleInstance, unsigned long x,
                 const unsigned long *dp) {
  char c;
  unsigned long d;
  if (x) {
    while (x < *dp)
      ++dp;
    do {
      d = *dp++;
      c = '0';
      while (x >= d)
        ++c, x -= d;
      myputc(moduleInstance, c);
    } while (!(d & 1));
  } else
    myputc(moduleInstance, '0');
}

static void puth(uint32_t moduleInstance, unsigned n) {
  static const char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  myputc(moduleInstance, hex[n & 15]);
}

void myprintf(const char *format, ...) {
  char c;
  int i;
  long n;

  va_list a;
  va_start(a, format);
  while ((c = *format++)) {
    if (c == '%') {
      switch (c = *format++) {
      case 's': // String
        myputs(MY_PRINTF_UART_BASE, va_arg(a, char *));
        break;
      case 'c': // Char
                /// @note You can't use va_arg(a, char) here, for va_arg()
                /// promotes char to int, hence an illegal hardware
                /// instruction error (tested on GCC x86 Linux).
        myputc(MY_PRINTF_UART_BASE, (char)va_arg(a, int));
        break;
      case 'i': // 16 bit Integer
      case 'u': // 16 bit Unsigned
        i = va_arg(a, int);
        if (c == 'i' && i < 0)
          i = -i, myputc(MY_PRINTF_UART_BASE, '-');
        xtoa(MY_PRINTF_UART_BASE, (unsigned)i, dv + 5);
        break;
      case 'l': // 32 bit Long
      case 'n': // 32 bit uNsigned loNg
        n = va_arg(a, long);
        if (c == 'l' && n < 0)
          n = -n, myputc(MY_PRINTF_UART_BASE, '-');
        xtoa(MY_PRINTF_UART_BASE, (unsigned long)n, dv);
        break;
      case 'x': // 16 bit heXadecimal
        i = va_arg(a, int);
        puth(MY_PRINTF_UART_BASE, i >> 12);
        puth(MY_PRINTF_UART_BASE, i >> 8);
        puth(MY_PRINTF_UART_BASE, i >> 4);
        puth(MY_PRINTF_UART_BASE, i);
        break;
      case 0:
        return;
      default:
        goto bad_fmt;
      }
    } else
    bad_fmt:
      myputc(MY_PRINTF_UART_BASE, c);
  }
  va_end(a);
}

/**
 * @brief Set up UART0.
 */
void uart_init(void) {
  /* Selecting P1.2 and P1.3 in UART mode */
  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
      GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3,
      GPIO_PRIMARY_MODULE_FUNCTION);

  /* Configuring UART Module */
  MAP_UART_initModule(MY_PRINTF_UART_BASE, &uartConfig);

  /* Enable UART module */
  MAP_UART_enableModule(MY_PRINTF_UART_BASE);
}

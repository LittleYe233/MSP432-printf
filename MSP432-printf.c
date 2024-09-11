/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include "printf.h"

int main(void) {
  /* Halting WDT  */
  MAP_WDT_A_holdTimer();

  /* Setting DCO to 12MHz */
  CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

  MAP_Interrupt_enableMaster();

  uart_init();

  /* Initialize values to display */
  char *s = "printf test";
  char c = '!';
  int i = -12345;
  unsigned u = 4321;
  long int l = -123456780;
  long unsigned n = 1098765432;
  unsigned x = 0xABCD;

  myprintf("String         %s\n", s);
  myprintf("Test!\n");
  myprintf("Char           %c\r\n", c);
  myprintf("Integer        %i\r\n", i);
  myprintf("Unsigned       %u\r\n", u);
  myprintf("Long           %l\r\n", l);
  myprintf("uNsigned loNg  %n\r\n", n);
  myprintf("heX            %x\r\n", x);

  while (1);
}

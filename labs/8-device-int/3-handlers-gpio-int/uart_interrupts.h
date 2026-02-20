#include "libc/circular.h"
// #include "libpi/objs/uart.o"

void uart_init_int(void);
void uart_putc_int(uint8_t c);
int uart_getc_int(void);
int uart_irq_handler(uint32_t pc);


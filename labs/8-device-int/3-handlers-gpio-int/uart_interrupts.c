#include "libc/circular.h"


#define AUX_ENABLES 0x20215004
#define AUX_MU_IO_REG 0x20215040
#define AUX_MU_IER_REG 0x20215044
#define AUX_MU_IIR_REG 0x20215048
#define AUX_MU_LCR_REG 0x2021504C
#define AUX_MU_MCR_REG 0x20215050
#define AUX_MU_LSR_REG 0x20215054
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_BAUD_REG 0x20215068
#define IRQ_ENABLE_1  0x2000B210

static cq_t tx_q;
static cq_t rx_q;
volatile unsigned tx_irq_count = 0;

void uart_init_int(void) {
    uart_init();

    cq_init(&tx_q, 1024);
    cq_init(&rx_q, 1024);

    PUT32(AUX_MU_IER_REG, 0b1);
    dev_barrier();
    PUT32(IRQ_ENABLE_1, (1 << 29)); 
    dev_barrier();

}
void uart_putc_int(uint8_t c) {
    while (cq_full(&tx_q)) {}

    cq_push(&tx_q, c);

    PUT32(AUX_MU_IER_REG, 0b11);

}
int uart_getc_int(void) {
    int val = -1;
    
    if (!cq_empty(&rx_q)) {
        val = cq_pop(&rx_q); 
    }

    return val;

}
int uart_irq_handler(uint32_t pc) {

    if (GET32(AUX_MU_IIR_REG) & 1) {
        return 0; 
    }
    
    while (uart_has_data()) {
        uint8_t data = GET32(AUX_MU_IO_REG) & 0xff;
        cq_push(&rx_q, data);
    }
    while (uart_can_put8() && !cq_empty(&tx_q)) {
        uint8_t c = cq_pop(&tx_q);
        PUT32(AUX_MU_IO_REG, c);
    }

    if (cq_empty(&tx_q)) {
        PUT32(AUX_MU_IER_REG, 0b01); 
    }
    tx_irq_count++;


    return 1;

}


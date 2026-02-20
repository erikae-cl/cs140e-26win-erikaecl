#include "rpi.h"
#include "uart_interrupts.h"
#include "test-interrupts.h"
#define SIZE 512

extern volatile unsigned tx_irq_count;

void test_uart_async(void) {

    while (uart_getc_int() != -1) { ; }

    // Dump 512 dots into the buffer (8 bytes)
    
    trace("Filling TX queue with %d bytes...\n", SIZE);
    
    for (int i = 0; i < SIZE; i++) {
        uart_putc_int('|');
        if (i == SIZE - 1) {
            uart_putc_int('\n');
        }
    }
    
    
    volatile int cpu_count = 0;
    
    // Keep counting as long as the UART is still busy 
    while (!(GET32(0x20215064) & 0b1000000000)) {
        cpu_count++;
        for(int i=0; i<100; i++); 
    }


    trace("CPU count: %d loops\n", cpu_count);
    

    if (tx_irq_count > 0) {
        trace("TX interrupts fired (%d)\n", tx_irq_count);
    } else {
        panic("No TX interrupts fired — not async!");
    }
}

void notmain() {
   
    uart_test_init();

    trace("beginning test uart loopback\n");
    for (volatile int i = 0; i < 10000000; i++) {
        asm volatile("nop");
    }
    
    test_uart_async();
}


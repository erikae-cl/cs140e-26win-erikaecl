#include "rpi.h"
#include "uart_interrupts.h"
#include "test-interrupts.h"

static const char test_str[] = "We're no strangers to love\nYou know the rules and so do I\nA full commitment's what I'm thinking of\nYou wouldn't get this from any other guy\nI just wanna tell you how I'm feeling\nGotta make you understand\nNever gonna give you up\nNever gonna let you down\nNever gonna run around and desert you\nNever gonna make you cry\nNever gonna say goodbye\nNever gonna tell a lie and hurt you\n";

static void test_uart_loopback(void) {
    unsigned n = strlen(test_str);
    char received[n + 1];

    for (int i = 0; i < n; i++) {
        uart_putc_int(test_str[i]);
    }

    for (int i = 0; i < n; i++) {
        int c;
        
        // trace("waiting for letter\n");
        while ((c = uart_getc_int()) == -1) {
    
        }
        // trace("got letter\n");
        received[i] = (char)c;
    }
   
    received[n] = '\0';

    trace("Received:\n%s", received);
}

void notmain() {
   
    trace("beginning test uart loopback\n");
    for (volatile int i = 0; i < 10000000; i++) {
        asm volatile("nop");
    }

    uart_test_init();
    test_uart_loopback();
}


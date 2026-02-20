// part 1: uses your GPIO code to blink a single LED connected to 
// pin 20.
//   - when run should keep blinking.
//   - to restart: you have to power-cycle the pi.
#include "rpi.h"
#include <string.h>

// must do init first.
static inline void cycle_cnt_init(void) {
    uint32_t in = 1;
    asm volatile("MCR p15, 0, %0, c15, c12, 0" :: "r"(in));
}

// read cycle counter: should add a write().
static inline uint32_t cycle_cnt_read(void) {
    uint32_t out;
    asm volatile("MRC p15, 0, %0, c15, c12, 1" : "=r"(out));
    return out;
}

void notmain(void) {
    enum { led = 14};

    cycle_cnt_init();


    char str[] = "hello world";

    gpio_set_output(led);
    

    cycle_cnt_init();
    gpio_set_on(led);
    unsigned int start = cycle_cnt_read();

    while (cycle_cnt_read() - start < 6076) {}


    for (int i = 0; i < 11; i++) {
       
        unsigned int value = (unsigned int)str[i];
        unsigned int mask = 0x0000001;
        
        gpio_set_off(led);

        unsigned int cur = cycle_cnt_read();
        while (cycle_cnt_read() - cur < 6076) {};

        for (int j = 0; j < 8; j++) {
            unsigned int last = cycle_cnt_read();
            if (value & mask) { 
                gpio_set_on(led);
            }
            else {
                gpio_set_off(led);
            }

            value = value >> 1;
            
            while (cycle_cnt_read() - cur < 6076 * (j + 2)) {};
       
        }

        gpio_set_on(led);
        while (cycle_cnt_read() - cur < 6076 * 10) {};
            
        
    }
    // gpio_set_on(led);
    // cycle_cnt_init();
    // while (cycle_cnt_read() < 6076) {}
    


        
}

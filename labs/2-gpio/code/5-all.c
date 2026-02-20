// part 1: uses your GPIO code to blink a single LED connected to 
// pin 20.
//   - when run should keep blinking.
//   - to restart: you have to power-cycle the pi.
#include "rpi.h"

void notmain(void) {
    enum { led1 = 47, led2 = 20, led3 = 27};

    //47 = 1 is off, 47 = 0 on
    gpio_set_output(led1);
    gpio_set_output(led2);
    gpio_set_output(led3);

    while(1) {
        gpio_set_off(led1);
        gpio_set_on(led2);
        gpio_set_on(led3);
        delay_cycles(1000000);
        gpio_set_on(led1);
        gpio_set_off(led2);
        gpio_set_off(led3);
        delay_cycles(1000000);
    }
}

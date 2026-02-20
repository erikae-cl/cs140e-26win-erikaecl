// engler, cs140 put your gpio-int implementations in here.
#include "rpi.h"

// in libpi/include: has useful enums.
#include "rpi-interrupts.h"

#define GPREN0 0x2020004C 
#define GPFEN0 0x20200058 
#define GPEDS0 0x20200040 


// returns 1 if there is currently a GPIO_INT0 interrupt, 
// 0 otherwise.
//
// note: we can only get interrupts for <GPIO_INT0> since the
// (the other pins are inaccessible for external devices).
int gpio_has_interrupt(void) {
    dev_barrier();
    uint32_t val = GET32(IRQ_pending_2) & 0b1 << 16;
    dev_barrier();
    return val != 0;
}

// p97 set to detect rising edge (0->1) on <pin>.
// as the broadcom doc states, it  detects by sampling based on the clock.
// it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
// *after* a 1 reading has been sampled twice, so there will be delay.
// if you want lower latency, you should us async rising edge (p99)
//
// also have to enable GPIO interrupts at all in <IRQ_Enable_2>
void gpio_int_rising_edge(unsigned pin) {
    if(pin>=32)
        return;
    uint32_t addr = GPREN0 + (pin / 32) * 4;
    uint32_t val = 1 << (pin % 32);
    dev_barrier();
    OR32(addr, val);
    dev_barrier();
    PUT32(IRQ_Enable_2, 0x1 << 17);
    dev_barrier();
}

// p98: detect falling edge (1->0).  sampled using the system clock.  
// similarly to rising edge detection, it suppresses noise by looking for
// "100" --- i.e., is triggered after two readings of "0" and so the 
// interrupt is delayed two clock cycles.   if you want  lower latency,
// you should use async falling edge. (p99)
//
// also have to enable GPIO interrupts at all in <IRQ_Enable_2>
void gpio_int_falling_edge(unsigned pin) {
    if(pin>=32)
        panic("illegal pin %d", pin);
    uint32_t addr = GPFEN0 + (pin / 32) * 4;
    uint32_t val = 1 << (pin % 32);
    dev_barrier();
    OR32(addr, val);
    dev_barrier();
    PUT32(IRQ_Enable_2, 0x1 << 17);
    dev_barrier();
}

// p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
// if you configure multiple events to lead to interrupts, you will have to 
// read the pin to determine which caused it.
int gpio_event_detected(unsigned pin) {
    if(pin>=32)
        return 0;
    uint32_t addr = GPEDS0 + (pin / 32) * 4;
    uint32_t mask = 1 << (pin % 32);
    dev_barrier();
    uint32_t result = (GET32(addr) & mask);
    dev_barrier();
    return result != 0;
}

// p96: have to write a 1 to the pin to clear the event.
void gpio_event_clear(unsigned pin) {
    if(pin>=32)
        return;
    uint32_t addr = GPEDS0 + (pin / 32) * 4;
    uint32_t mask = 1 << (pin % 32);
    dev_barrier();
    PUT32(addr, mask);
    dev_barrier();
}

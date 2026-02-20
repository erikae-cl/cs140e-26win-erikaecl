#define GPREN0 0x2020004C 
#define GPFEN0 0x20200058 
#define GPEDS0 0x20200040 


static void or32(volatile void *addr, uint32_t val) {
    device_barrier();
    put32(addr, get32(addr) | val);
    device_barrier();
}

static void OR32(uint32_t addr, uint32_t val) {
    or32((volatile void*)addr, val);
}
// p97 set to detect rising edge (0->1) on <pin>.
// as the broadcom doc states, it  detects by sampling based on the clock.
// it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
// *after* a 1 reading has been sampled twice, so there will be delay.
// if you want lower latency, you should us async rising edge (p99)
void gpio_int_rising_edge(unsigned pin) {
    uint32_t addr = GPREN0 + (pin / 32) * 4;
    uint32_t mask = 1 << (pin % 32);
    OR32(addr, mask);
}


// p98: detect falling edge (1->0).  sampled using the system clock.  
// similarly to rising edge detection, it suppresses noise by looking for
// "100" --- i.e., is triggered after two readings of "0" and so the 
// interrupt is delayed two clock cycles.   if you want  lower latency,
// you should use async falling edge. (p99)
void gpio_int_falling_edge(unsigned pin) {
    uint32_t addr = GPFEN0 + (pin / 32) * 4;
    uint32_t mask = 1 << (pin % 32);
    OR32(addr, mask);
}

// p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
// if you configure multiple events to lead to interrupts, you will have to 
// read the pin to determine which caused it.
int gpio_event_detected(unsigned pin) {
    uint32_t addr = GPEDS0 + (pin / 32) * 4;
    uint32_t mask = 1 << (pin % 32);
    device_barrier();
    return (GET32(addr) & mask) != 0;
}

// p96: have to write a 1 to the pin to clear the event.
void gpio_event_clear(unsigned pin) {
    uint32_t addr = GPEDS0 + (pin / 32) * 4;
    uint32_t mask = 1 << (pin % 32);
    device_barrier();
    PUT32(addr, mask);
    device_barrier();
}
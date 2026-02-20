#include "rpi.h"
#include "mbox.h"

uint32_t rpi_temp_get(void) ;

#include "cycle-count.h"

#define GPIO_LEV0 0x20200034
uint32_t gpio_rd_usec(unsigned n) {
    volatile uint32_t *addr = (void*)GPIO_LEV0;

    let s = timer_get_usec();
    asm volatile(".align 5");
    for(unsigned i = 0; i < n/8; i++) {
        *addr;
        *addr;
        *addr;
        *addr;

        *addr;
        *addr;
        *addr;
        *addr;
    }

    for(unsigned i = 0; i < n%8; i++)
        *addr;
    let tot = timer_get_usec() - s;

    return tot;
}

uint32_t gpio_wr_usec(unsigned n) {
    volatile uint32_t *addr = (void*)GPIO_LEV0;

    let s = timer_get_usec();
    asm volatile(".align 5");
    for(unsigned i = 0; i < n/8; i++) {
        *addr = 1;
        *addr = 1;
        *addr = 1;
        *addr = 1;

        *addr = 1;
        *addr = 1;
        *addr = 1;
        *addr = 1;
    }

    for(unsigned i = 0; i < n%8; i++)
        *addr;
    let tot = timer_get_usec() - s;

    return tot;
}

// compute cycles per second using
//  - cycle_cnt_read();
//  - timer_get_usec();
unsigned cyc_per_sec(void) {
    // mem write 10000 times  = 318 usec (31.8ns/op)
    // read mem 10000 times   = 702 usec (70.2ns / op)
    // w/r mem 10000 times    = 964 usec (96.4ns/op)
    // GPIO write 10000 times = 501 usec (50.1ns/op)
    // read GPIO 10000 times  = 926 usec (92.6ns / op)
    int array[10000];
    uint32_t start = timer_get_usec();
    for (int i = 0; i < 10000; i++) {
        array[i] = i * 2;
    }
    uint32_t end = timer_get_usec();
    output("mem write 10000 times = %u\n", end - start);

    start = timer_get_usec();
    for (int i = 0; i < 10000; i++) {
        int val = array[i];
    }
    end = timer_get_usec();
    output("mem read 10000 times = %u\n", end - start);

    start = timer_get_usec();
    for (int i = 0; i < 10000; i++) {
        array[i]++;
    }
    end = timer_get_usec();
    output("mem r/w 10000 times = %u\n", end - start);

   uint32_t gpio_wr_t = gpio_wr_usec(10000);
   output("gpio write 10000 times = %u\n", gpio_wr_t);
    
    uint32_t gpio_read_t = gpio_rd_usec(10000);
   
    output("gpio read 10000 times = %u\n", gpio_read_t);
    
    


    unsigned cyc_start = cycle_cnt_read();

    unsigned s = timer_get_usec();
    while((timer_get_usec() - s) < 1000*1000)
        ;
    unsigned cyc_end = cycle_cnt_read();

    unsigned tot = cyc_end - cyc_start;
    printk("cycles per sec = %u\n", tot);

    return tot;
}

char clock_names[15][10] = {"reserved", "EMMC", "UART", "ARM", "CORE", "V3D", "H264", "ISP", "SDRAM", "PIXEL", "PWM", "HEVC", "EMMC2", "M2MC", "PIXEL_BVB"};


void notmain(void) { 
    output("mailbox serial number = %llx\n", rpi_get_serialnum());
    output("mailbox model number = %x\n", rpi_get_model());
   

    output("mailbox revision number = %x\n", rpi_get_revision());

    uint32_t size = rpi_get_memsize();
    output("mailbox physical mem: size=%d (%dMB)\n", 
            size, 
            size/(1024*1024));

    // print as fahrenheit
    unsigned x = rpi_temp_get();

    // convert <x> to C and F
    unsigned C = x/1000, F = (9 * C) / 5 + 32;
    output("mailbox temp = %x, C=%d F=%d\n", x, C, F); 
    cyc_per_sec();
    caches_enable();
    for (int i = 1; i < 15; i++) {
        output("Clock %s info:\n", clock_names[i]);
        uint32_t max = rpi_clock_maxhz_get(i);
        output("Max rate = %u\n", max);
        output("Clock rate = %u\n", rpi_clock_curhz_get(i));
        
        output("Measured rate = %u\n", rpi_clock_realhz_get(i));
        output("Set rate = %u\n", rpi_clock_hz_set(i, max));
        
        output("Clock rate = %u\n", rpi_clock_realhz_get(i));
        
    }
    cyc_per_sec();
}

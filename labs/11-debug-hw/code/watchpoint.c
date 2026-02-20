// very dumb, simple interface to wrap up watchpoints better.
// only handles a single watchpoint.
#include "rpi.h"
#include "watchpoint.h"

#include "../../libpi/include/asm-helpers.h"
// keep track of what we are watching.
static uint32_t watch_addr;

// was it a watchpoint fault?
//  1. use dfsr 3-64  to make sure it was a debug event.
//  2. and dscr 13-11: to make sure it was a watchpoint
cp_asm(dfsr, p15, 0, c5, c0, 0);
cp_asm(dscr, p14, 0, c0, c1, 0);
cp_asm(wfar, p14, 0, c0, c6, 0);
cp_asm(far, p15, 0, c6, c0, 0);

cp_asm(wcr0, p14, 0, c0, c0, 7);
cp_asm(wvr0, p14, 0, c0, c0, 6);

int watchpt_fault_p(void) {
    // return staff_watchpt_fault_p();

    // cp_asm(debug_status, p15, 0, c5, c0, 0);
    uint32_t debug_event = dfsr_get();
    uint32_t bit_ten = debug_event & 0x1 << 10;
    uint32_t event = debug_event & 0b1111;
    if ((bit_ten == 0) && ((event == 0b0010))) {
        uint32_t dscr = dscr_get();
        uint32_t watchp = dscr & 0b111100;
        if (watchp == 0b001000) {
            return 1;
        }
    }
    return 0;
}

// is it a load fault?
//  - use dfsr 3-64
int watchpt_load_fault_p(void) {
    if(!watchpt_fault_p())
        return 0;
    // return staff_watchpt_load_fault_p();
    uint32_t load_fault = dfsr_get();
    uint32_t bit_ten = load_fault & 0x1 << 10;
    uint32_t event = load_fault & 0x1 << 11;
    if ((bit_ten == 0) && ((event == 0))) {
        return 1;
    }
    return 0;

}

// get the pc of the fault.
//   - p13-34: use <wfar> (see 3-12) to get the fault pc 
// important:
//   - pay attention to the comment on 13-12 to see how to adjust!
uint32_t watchpt_fault_pc(void) {
    // return staff_watchpt_fault_pc();

    uint32_t pc = wfar_get();
    return pc - 0x8;

}

// get the data address that caused the fault.
// use <far> 3-68 to get the fault addr.
uint32_t watchpt_fault_addr(void) {
    // return staff_watchpt_fault_addr();
    return far_get();

}

// set a watch-point on <addr>: 
//  1. enable cp14 if not enabled.  
//     - MAKE SURE TO DO THIS FIRST.
//  2. set wcr0 (13-21), wvr0 (13-20)
//     - don't rmw -- just set it directly.
// Important: 
//  - make sure you handle subword accesses! 
int watchpt_on(uint32_t addr) {
    watch_addr = addr;
    // return staff_watchpt_on(addr);
    uint32_t cur_val = dscr_get();
    uint32_t mask = ~(0b11 << 14);
    dscr_set((cur_val & mask) | 0b10 << 14);
    // prefetch_flush();
    wvr0_set(watch_addr);

    wcr0_set(0b111111111);
    prefetch_flush();
    return 1;
  
}

// turn off watchpoint:
//   - check that <addr> is what we were watching.
//   - clear wcr0
int watchpt_off(uint32_t addr) {
    if(addr != watch_addr)
        panic("disabling invalid watchpoint %x, tracking %x\n", 
            addr, watch_addr);
    // return staff_watchpt_off(addr);
    wcr0_set(0);
    return 1;
}

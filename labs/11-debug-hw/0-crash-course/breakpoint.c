#include "rpi.h"
#include "rpi-interrupts.h"
#include "asm-helpers.h"
#include "bit-support.h"
#include "breakpoint.h"

cp_asm(dscr, p14, 0, c0, c1, 0);
cp_asm(bcr1, p14, 0, c0, c1, 5);
cp_asm(bvr1, p14, 0, c0, c1, 4);
cp_asm(bcr0, p14, 0, c0, c0, 5);
cp_asm(bvr0, p14, 0, c0, c0, 4);
cp_asm(ifsr, p15, 0, c5, c0, 1);


// make sure that cp14 (DSCR) is enabled (p13-7)
void brkpt_match_init(void) {
    // staff_brkpt_match_init();
    uint32_t cur_val = dscr_get();
    uint32_t mask = ~(0b11 << 14);
    dscr_set((cur_val & mask) | 0b10 << 14);

}

// set match on <addr>
//
// for simplicity: for matching we use breakpoint 1 
// (bcr1 p13-17 and bvr1 p13-16) so dont't conflict 
// with single-stepping.   
void brkpt_match_set(uint32_t addr) {
    // staff_brkpt_match_set(addr);
    bvr1_set(addr);
    bcr1_set(0b111100111);
    prefetch_flush();

}

// turn off match faults (clear bcr1)
void brkpt_match_stop(void) {
    // staff_brkpt_match_stop();
    bcr1_set(0);
    // bvr1_set(0);
    prefetch_flush();
}

// return the match addr (bvr1)
uint32_t brkpt_match_get(void) {
    if (bcr1_get() & 0b1) {
        return bvr1_get();
    }
    return 0;
}


// set mismatch on <addr>
//
// for simplicity: for matching we use breakpoint 0. 
// so set bcr0 and bvr0.
void brkpt_mismatch_set(uint32_t addr) {
    // staff_brkpt_mismatch_set(addr);
    bcr0_set(0);
    bvr0_set(addr);
    bcr0_set(0b111100111 | 0b10 << 21);
    prefetch_flush();

}

// this will mismatch on the first instruction at user level.
void brkpt_mismatch_start(void) {
    // 1. check DSCR: if not enabled, enable it.
    // 2. set brkpt_mismatch_set(0)
    // staff_brkpt_mismatch_start();

    // uint32_t cur_val = dscr_get();
    brkpt_match_init();

    brkpt_mismatch_set(0);
    prefetch_flush();
}

// turn off mismatching: clear bcr0
void brkpt_mismatch_stop(void) {
    // staff_brkpt_mismatch_stop();
    bcr0_set(0);
    prefetch_flush();
}

// was this a breakpoint fault? (either mismatch or match)
// check IFSR bits (p 3-66) to see it was a debug fault.
// check DSCR bits (13-11) to see if it was a breakpoint
int brkpt_fault_p(void) {
    // return staff_brkpt_fault_p();
    uint32_t ifsr_val = ifsr_get();
    uint32_t last_three = ifsr_val & 0b1111;
    if (last_three == 0b0010) {
        uint32_t dscr_val = dscr_get();
        uint32_t breakp = dscr_val & 0b111100;
        if (breakp == 0b000100) {
            return 1;
        }
    }

    return 0;
}

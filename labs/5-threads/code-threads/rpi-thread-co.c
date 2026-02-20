#include "rpi.h"
#include "rpi-thread-copy.h"

static co_th_t *cur_thread;

co_th_t *main_thread;

static unsigned tid = 1;

void co_cswitch(uint32_t *old_regs, uint32_t *new_regs);

// create new co-routine and return the initialized structure
co_th_t co_mk(void (*fn)(uint32_t), uint32_t arg, uint32_t sp) {
    void rpi_init_trampoline(void);

    co_th_t t;
    t.fn = fn;
    t.arg = arg;

   
    t.regs[R4_OFFSET] = arg;
    t.regs[R5_OFFSET] = (unsigned) fn;
    t.regs[SP_OFFSET] = sp;
    t.regs[LR_OFFSET] = (unsigned) &rpi_init_trampoline;
    t.tid = tid++;
    return t;
}

// switch from co-routine <old> to <new>
void co_switch(co_th_t *old, co_th_t *new) {
    // trace("switching from tid=%d to tid=%d\n", old->tid, new->tid);
    if (main_thread == NULL) {
        main_thread = old;
    }
    cur_thread = new;
    co_cswitch(old->regs, new->regs);
}


void co_done() {
    trace("all work was done!\n", cur_thread->tid);
    co_switch(cur_thread, main_thread);
} 

void rpi_exit(int i) {
    co_done();
}
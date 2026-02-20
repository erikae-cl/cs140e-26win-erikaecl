// run N threads that yield and explicitly call exit.
#include "test-header.h"

#define num_routine 2
static co_th_t *cur_thread;

co_th_t threads[num_routine];
uint32_t stack[num_routine][THREAD_MAXSTACK];



static void co1(uint32_t arg) {
    for (int i = 0; i < 10; i++) {
        trace("A %d\n", i);
        co_switch(&threads[0], &threads[1]);
    }
    trace("co %d is reporting done!\n", arg);
    co_done();
}

static void co2(uint32_t arg) {
    for (int i = 0; i < 10; i++) {
        trace("B %d\n", i);
        co_switch(&threads[1], &threads[0]);
    }
    trace("co %d is reporting done!\n", arg);
    co_done();
}


static void trivial(uint32_t arg) {
    trace("coroutine started with id %d\n", arg);
    trace("coroutine done with id %d\n", arg);
    
    co_done();
}

void notmain(void) {
    test_init();

    co_th_t main_thread; 

    main_thread.tid = 0;

    threads[0] = co_mk(co1, 1, (uint32_t) &stack[0][THREAD_MAXSTACK]);
    threads[1] = co_mk(co2, 2, (uint32_t) &stack[1][THREAD_MAXSTACK]);

    trace("Switching to coroutine 1\n");
    co_switch(&main_thread, &threads[0]);  // start co1

    // trace("Switching to coroutine 1\n");
    // co_switch(&main_thread, &threads[1]);  // start co2

    test_done();
    trace("SUCCESS\n");
}



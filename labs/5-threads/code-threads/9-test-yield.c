// run N threads that yield and explicitly call exit.
#include "test-header.h"

static co_th_t threads[2];
uint32_t stack[2][THREAD_MAXSTACK];
co_th_t main_thread;
int itr = 540;

void trivial1(uint32_t arg) {
    for (int i = 0; i < itr; i++) {
        printk("/");
        co_yield();
    }
    co_done();


}

void trivial2(uint32_t arg) {
    for (int i = 0; i < itr; i++) {
        if (i == itr - 1) {
            printk("\\\n");
        } else{
            printk("\\");
        }
        co_yield();
    }
    co_done();
}

void notmain(void) {
    test_init();

   
    main_thread.tid = 0;
    threads[0] = co_mk(trivial1, 1, (uint32_t)&stack[0][THREAD_MAXSTACK]);
    threads[1] = co_mk(trivial2, 2, (uint32_t)&stack[1][THREAD_MAXSTACK]);
    co_th_t* co1 = &threads[0];
    co_th_t* co2 = &threads[1];


    for (int i = 0; i < itr + 1; i++) {
        co_switch(&main_thread, co1);
        co_switch(&main_thread, co2);

    }
  
    test_done();
    trace("SUCCESS\n");
}

// run N threads that yield and explicitly call exit.
#include "test-header.h"

#define num_routine 2
static co_th_t *cur_thread;

co_th_t threads[num_routine];
uint32_t stack[num_routine][THREAD_MAXSTACK];

co_th_t *co1 = &threads[0];
co_th_t *co2 = &threads[1];

int itr = 15;

void left(uint32_t arg) {
    for (int i = 0; i < itr * 3; i++) {
    
        if (i == 0) {
            for (int j = (itr * 3 - 1); j >= 0; j--) {
                printk(" ");
            }
            printk("(-_-)\r\n\r\n");

        }

        for (int j = (itr * 3 - i); j >= 0; j--) {
            printk(" ");
        }

        printk("/");
        if (i == itr) {
            for (int k = 0; k < i; k++) {
                co_switch(co1, co2);
                printk("/");
            }   
        } 
        co_switch(co1, co2);
    }
    co_done();


}

void right(uint32_t arg) {
    for (int i = 0; i < itr * 3; i++) {
        if (i == itr) {
            for (int k = 0; k < i; k++) {
                printk("\\");
                co_switch(co2, co1);
            }
            printk("\\\n");
            co_switch(co2, co1);
            continue;
        } 

        for (int j = 0; j < i * 2; j++) {
            printk(" ");
        }

        printk("\\\n");
        
        co_switch(co2, co1);
    }
    co_switch(co2, co1);
}

void notmain(void) {
    test_init();

   
    co_th_t main_thread; 
    
    main_thread.tid = 0;


    threads[0] = co_mk(left, 1, (uint32_t) &stack[0][THREAD_MAXSTACK]);
    threads[1] = co_mk(right, 2, (uint32_t) &stack[1][THREAD_MAXSTACK]);


    co_switch(&main_thread, co1);
  
    test_done();
    trace("SUCCESS\n");
}

#include "test-header.h"

unsigned thread_count = 0;
unsigned cumulative_error = 0;
static uint32_t errors[10];
static uint32_t times[10];
static uint32_t base_time = 5000;
static uint32_t incr = 2000;

static void sleeper_thread(void *arg) {
    uint32_t sleep_time_us = *(uint32_t *)arg;
    uint32_t start = timer_get_usec();
    sleep_until_us(sleep_time_us);
    uint32_t actual = timer_get_usec();
    uint32_t error = (actual) - (start + sleep_time_us);

    int id = ((*(uint32_t*)arg) - base_time) / incr; 
    errors[id] = actual - (start + sleep_time_us);
    times[id] = actual - start;
    
    thread_count++;
    rpi_exit(0);
}

void notmain() {
    test_init();

    thread_count = 0;
    cumulative_error = 0;

    const int n_threads = 10;
    uint32_t sleep_times[n_threads];

    for(int i = 0; i < n_threads; i++) {
        sleep_times[i] = base_time + i * incr; // 5 ms, 6 ms, 7 ms, ...
        uint32_t *arg = kmalloc(sizeof(*arg));
        *arg = sleep_times[i];
        rpi_fork(sleeper_thread, arg);
    }

    rpi_thread_start();

    trace("all threads finished: count=%d\n", thread_count);
    for(int i=0; i<n_threads; i++) {
        trace("Thread %d[requested:%u, actual:%u, error: %u]\n", i, base_time+incr*i, times[i], errors[i]);
        cumulative_error += errors[i];
    }
    trace("cumulative wakeup error = %u us\n", cumulative_error);

    assert(thread_count == n_threads);
    
    test_done();
    trace("SUCCESS!\n");
}
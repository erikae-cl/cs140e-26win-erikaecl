#include "check-interleave.h"

/* * The Goal: Prove that a simple "check-then-set" lock is not thread-safe.
 * The Checker should find an interleaving where both A and B enter the
 * critical section simultaneously.
 */

static volatile int lock_flag = 0;
static volatile int shared_resource = 0;
static int bug_found = 0;

// 1. Reset state before every trial run.
static void lock_init(checker_t *c) {
    lock_flag = 0;
    shared_resource = 0;
    bug_found = 0;
}

// 2. Routine A: The "Victim" thread.
// We purposely write this in a way that separates the 'read' from the 'write'.
static void lock_A(checker_t *c) {
    if (lock_flag == 0) {       
        // <--- If the single-step handler switches to B HERE, we fail.
        lock_flag = 1;          
        
        // Critical Section
        shared_resource++;
        if (shared_resource > 1) {
            bug_found = 1;
        }
        shared_resource--;
        
        lock_flag = 0;
    }
}

// 3. Routine B: The "Interrupter" thread.
// This is the routine your checker will try to "inject" at every 
// instruction boundary of A.
static int lock_B(checker_t *c) {
    // B only attempts to enter if the lock looks free.
    if (lock_flag == 0) {
        lock_flag = 1;
        
        // Critical Section
        shared_resource++;
        if (shared_resource > 1) {
            bug_found = 1;
        }
        shared_resource--;
        
        lock_flag = 0;
        return 1; // Success: B ran.
    }
    return 0; // Failure: Lock was busy, checker will try again later.
}

// 4. Verification: Check if the safety property was violated.
static int lock_check(checker_t *c) {
    if (bug_found) {
        output("  [!] BUG DETECTED: A and B were in the critical section at the same time!\n");
        return 0; // Return 0 to tell the checker a bug was found.
    }
    return 1;
}


void not_main(void) {
    // 1. Hardware/Exception Setup (Specific to your rpi environment)
    // full_except_install() is usually called inside your check() function,
    // but ensure your UART/printk is ready.
    
    output("------------------------------------------------\n");
    output("Checking: %s\n", lock_test.name);
    output("------------------------------------------------\n");

    // 2. Run the checker
    // This will loop, incrementing the instruction switch point 'i' 
    // until it finds the race or runs out of instructions.
    if (check(&lock_test)) {
        output("SUCCESS: Checker finished. No interleaving broke the code.\n");
    } else {
        output("RESULT: Checker successfully found and reported a race condition.\n");
    }
}
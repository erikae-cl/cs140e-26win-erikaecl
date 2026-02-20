#ifndef CO_H
#define CO_H

#include <stdint.h>
#include "rpi.h"
#include "rpi-thread.h"
#include <stdbool.h>

// offsets for saved registers
enum {
    R4_OFFSET = 0,
    R5_OFFSET = 1,
    R6_OFFSET,
    R7_OFFSET,
    R8_OFFSET,
    R9_OFFSET,
    R10_OFFSET,
    R11_OFFSET,
    SP_OFFSET = 8,
    LR_OFFSET = 9,
};

// Coroutine structure
typedef struct co_th{
    uint32_t regs[10];   
    uint32_t *saved_sp;    
    uint32_t tid;                   
    void (*fn)(uint32_t);            
    uint32_t arg;                     
} co_th_t;

// create a new coroutine
co_th_t co_mk(void (*fn)(uint32_t), uint32_t arg, uint32_t sp);

// switch between coroutines
void co_switch(co_th_t *old, co_th_t *new);

void co_yield(void);

void co_trampoline(void);

// called when a coroutine finishes
void co_done(void);

#endif // CO_H

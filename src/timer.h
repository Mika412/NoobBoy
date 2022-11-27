#pragma once

#include "mmu.h"
#include "interrupt.h"

class Timer{
    Registers *registers;
    MMU *mmu;
    Interrupts *interrupts;

    int gameboy_ticks = 4 * 1024 * 1024; // 4194304
    
    int div = 0;
    int tac = 0;
    int tima = 0;

    void check();

    public:
        Timer(MMU *memory, Interrupts* interrupts);

        void inc();
};

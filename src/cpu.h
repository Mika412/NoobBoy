#pragma once

#include "Registers.h"
#include "mmu.h"
#include "interrupt.h"
#include "instructions.h"

class CPU {
    InstructionSet* instructions;
    MMU *memory;
    Interrupts *interrupts;

    public:
        Registers *registers;

        bool stopped = false;
        int instrs_count = 0;

        CPU(Registers *registers, Interrupts* interrupts, MMU* memory);

        void reset();
        bool step();
        void no_bootrom_init();
        void print_flags();
        void print_debug();
};

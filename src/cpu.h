#pragma once

#include "registers.h"
#include "mmu.h"
#include "interrupt.h"
#include "instructions.h"

class CPU {
    InstructionSet* instructions;
    MMU *memory;
    Interrupts *interrupts;
    Registers *registers;

    public:
        CPU(Registers *registers, Interrupts* interrupts, MMU* memory);

        void reset();
        void step();
        void no_bootrom_init();
        void print_flags();
        void print_debug();
};

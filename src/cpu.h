#pragma once

#include "Registers.h"
#include "mmu.h"
#include "interrupt.h"
#include "instructions.h"

class CPU {
    private:
        InstructionSet* instructions;
        MMU *memory;
        Interrupts *interrupts;

    public:
        Registers *registers;

        bool stopped = false;
        uint8_t last_instruction = 0x00;
        

        struct clock {
            unsigned long m = 0; // Total quarter clock
            unsigned long t = 0; // Total full clock
        } clock;

        CPU(Registers *registers, Interrupts* interrupts, MMU* memory);

        void reset();
        bool step();
        void no_bootrom_init();
        void print_flags();
        void print_debug();
};

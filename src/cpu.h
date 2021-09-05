#pragma once

#include "instructions.h"
#include "Registers.h"
#include "Memory.h"

class CPU {
    private:
        InstructionSet *instructions;
        MemoryBus *memory;

    public:
        Registers *registers;

        bool stopped = false;
        uint8_t last_instruction = 0x00;
        

        struct clock {
            unsigned long m = 0; // Total quarter clock
            unsigned long t = 0; // Total full clock
        } clock;

        CPU(Registers *registers, MemoryBus* cmemory);

        void reset();
        bool step();
        void print_flags();
        void print_debug();
};

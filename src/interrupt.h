#pragma once

#include "cpu.h"
#include "Memory.h"

class Interrupt {
    Registers *registers;
    MemoryBus *memory;

    public:
        Interrupt(Registers *registers, MemoryBus *memory);
        bool step();

};

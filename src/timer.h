#pragma once

#include "Registers.h"
#include "Memory.h"

class Timer{
    public:
        Registers *registers;
        MemoryBus *memory;
        int main = 0;
        int sub = 0;
        int div = 0;

        Timer(Registers *registers, MemoryBus *memory);
        void step();
        void check();
        void inc();
        /* void rb();
        void wb(); */
};

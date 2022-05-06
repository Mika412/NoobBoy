#pragma once

#include "Registers.h"
#include "mmu.h"

class Timer{
    public:
        Registers *registers;
        MMU *memory;
        int main = 0;
        int sub = 0;
        int div = 0;

        Timer(Registers *registers, MMU *memory);
        void step();
        void check();
        void inc();
        /* void rb();
        void wb(); */
};

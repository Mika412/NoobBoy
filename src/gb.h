#include "cpu.h"
#include "gpu.h"
#include "timer.h"
#include "renderer.h"
#include "interrupt.h"

class GB{
    public:
        Registers registers;
        MMU mmu;
        CPU *cpu;
        GPU *gpu;
        Interrupts *interrupts;
        Timer *timer;
        Renderer *renderer;

        bool render = true;
        bool debug = false;
        // bool debug = true;

    void init();
    void run();
};

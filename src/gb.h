#include "cpu.h"
#include "gpu.h"
#include "timer.h"
#include "renderer.h"
#include "interrupt.h"

class GB{
    public:
        Registers registers;
        MemoryBus *memory;
        CPU *cpu;
        GPU *gpu;
        Interrupt *interrupt;
        Timer *timer;
        Renderer *renderer;

        bool render = true;
        bool debug = false;
        // bool debug = true;

    void init();
    void run();
};

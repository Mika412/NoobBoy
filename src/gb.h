#include "cpu.h"
#include "gpu.h"
#include "timer.h"
#include "renderer.h"
#include "interrupt.h"
#include "joypad.h"

class GB{
    public:
        Registers registers;
        MMU mmu;
        CPU *cpu;
        GPU *gpu;
        Interrupts *interrupts;
        Timer *timer;
        Renderer *renderer;
        Joypad *joypad;

        bool render = true;
        bool debug = true;
        bool doStep = false;
        bool isPaused = false;
        bool isRunning = true;

    void init(std::string rom, std::string bootrom, bool debug);
    void run();
};

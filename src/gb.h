// #include <cstring>
#include "cpu.h"
#include "ppu.h"
#include "timer.h"
#include "renderer.h"
#include "interrupt.h"
#include "joypad.h"
#include "status.h"

class GB{
    public:
        Status status;

        Registers registers;
        MMU mmu;
        CPU *cpu;
        PPU *ppu;
        Interrupts *interrupts;
        Timer *timer;
        Renderer *renderer;
        Joypad *joypad;

    void init(std::string rom, std::string bootrom, bool debug);
    void run();
};

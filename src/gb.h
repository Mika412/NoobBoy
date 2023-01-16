// #include <cstring>
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "timer.h"
#include "renderer.h"
#include "cartridge.h"
#include "interrupt.h"
#include "joypad.h"
#include "status.h"

#include <iostream>
#include <cstdint>
#include <unistd.h>
#include <set>
#include <chrono>
#include <thread>
#include <getopt.h>

class GB{
    public:
        Status status;

        Registers registers;
        MMU *mmu;
        CPU *cpu;
        PPU *ppu;
        APU *apu;
        Interrupts *interrupts;
        Timer *timer;
        Renderer *renderer;
        Joypad *joypad;

    void init(std::string rom, std::string bootrom = "", std::string save_file = "", bool debug = false, bool sound = false);
    void init(Cartridge *cartridge, std::string bootrom = "", bool debug = false, bool sound = false);
    void run();
};

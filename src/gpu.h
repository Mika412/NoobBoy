#pragma once

#include "Registers.h"
#include "mmu.h"
#include "interrupt.h"

struct rgb {
    unsigned char r, g, b, a;
};
    
    typedef struct rgb COLOUR;

const COLOUR palette[4] = {
        { 255, 255, 255, 255},
        // { 0, 192, 192, 192 },
        // { 0, 96, 96, 96 },
        // { 0, 255, 255, 255 },
        // { 0, 255, 255, 255 },
        { 192,192,192,255},
        { 96,96,96, 255 },
        { 0, 0, 0, 255 },
    };


class GPU{
    public:
        Registers* registers;
        MMU* memory;
        Interrupts *interrupts;
        uint8_t* control;
        uint8_t* scrollX;
        uint8_t* scrollY;
        uint8_t* scanline;
        unsigned long tick;

        COLOUR framebuffer[160 * 144];
        unsigned short background[2000];
        // unsigned short background[32 * 32];

        int mode = 0;
        int modeclock = 0;

    GPU(Registers *registers, Interrupts* interrupts, MMU *memory);
    void step(); 
    void render_scan_lines(); 
};

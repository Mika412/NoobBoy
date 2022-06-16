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


class PPU{
    public:
        Registers* registers;
        MMU* mmu;
        Interrupts *interrupts;
        uint8_t* scrollX;
        uint8_t* scrollY;
        uint8_t* scanline;
        unsigned long tick;

        struct Control
        {
            union
            {
                struct {
                    uint8_t bgDisplay : 1;
                    uint8_t spriteDisplayEnable : 1;
                    uint8_t spriteSize : 1;
                    uint8_t bgDisplaySelect : 1;
                    uint8_t bgWindowDataSelect : 1;
                    uint8_t windowEnable : 1;
                    uint8_t windowDisplaySelect : 1;
                    uint8_t lcdEnable : 1;
                };
            };
        }*control;

        COLOUR framebuffer[160 * 144];
        unsigned short background[2000];
        // unsigned short background[32 * 32];

        int mode = 1;
        int modeclock = 0;

        bool can_render = false;

    PPU(Registers *registers, Interrupts* interrupts, MMU *mmu);
    void step(); 
    void compare_ly_lyc(); 

    void render_scan_lines(); 
    void render_scan_line_background(bool* row_pixels); 
    void render_scan_line_window(); 
    void render_scan_line_sprites(bool* row_pixels); 
};

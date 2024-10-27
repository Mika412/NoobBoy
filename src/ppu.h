#pragma once

#include "registers.h"
#include "mmu.h"
#include "interrupt.h"

class PPU {
    void render_scan_lines();
    void render_scan_line_background(bool *row_pixels);
    void render_scan_line_window();
    void render_scan_line_sprites(bool *row_pixels);

   public:
    Registers *registers;
    MMU *mmu;
    Interrupts *interrupts;
    uint8_t *scrollX;
    uint8_t *scrollY;
    uint8_t *scanline;
    uint32_t tick;

    struct Control {
        union {
            struct {
                uint8_t bgDisplay : 1;
                uint8_t spriteDisplayEnable : 1;
                uint8_t spriteSize : 1;  // True means 8x16 tiles
                uint8_t bgDisplaySelect : 1;
                uint8_t bgWindowDataSelect : 1;
                uint8_t windowEnable : 1;
                uint8_t windowDisplaySelect : 1;
                uint8_t lcdEnable : 1;
            };
        };
    } *control;

    struct Stat {
        union {
            struct {
                uint8_t mode_flag : 2;
                uint8_t coincidence_flag : 1;
                uint8_t hblank_interrupt : 1;
                uint8_t vblank_interrupt : 1;
                uint8_t oam_interrupt : 1;
                uint8_t coincidence_interrupt : 1;
            };
        };
    } *stat;

    Colour framebuffer[160 * 144];
    // WTF: This variable is unused, but removing her breaks some games
    uint8_t background[32 * 32];

    int mode = 0;
    int modeclock = 0;

    bool can_render = false;

    PPU(Registers *registers, Interrupts *interrupts, MMU *mmu);
    void step();
    void compare_ly_lyc();
};

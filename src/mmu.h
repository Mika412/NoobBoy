#pragma once

#include <iostream>

#include "Registers.h"

struct Colour {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint8_t colours[4];
    };
};

class MMU {
    public:
        uint8_t memory[0xFFFF];

        int mbcType = 0;
        bool mbcRamEnabled = false;
        // bool mbc2ramEnabled = false;
        uint8_t mbcRomMode = 0;
        uint8_t mbcRomNumber = 1;
        uint8_t mbcRamNumber = 0;
        // uint8_t mbc2romNumber = 0;
        uint8_t ROMbanks[0x7F][0x4000];
        uint8_t RAMbanks[0x7F][0x2000];
        uint8_t banksLoaded;

        uint8_t joypad = 0xFF;

        struct clock {
            int m = 0;
            int t = 0;
            int t_prev = 0;
            int t_instr = 0;
            int last_instr = 0;
        } clock;

        struct timer{
            uint16_t div = 0;
            unsigned char tima = 0;
            unsigned char tma = 0;
            unsigned char tac = 0;
        } timer;

        struct Sprite {
            bool ready;
            uint8_t y;
            uint8_t x;
            uint8_t tile;
            Colour *colourPalette;
            struct {
                union {
                    struct {
                        uint8_t gbcPaletteNumber1 : 1;
                        uint8_t gbcPaletteNumber2 : 1;
                        uint8_t gbcPaletteNumber3 : 1;
                        uint8_t gbcVRAMBank: 1;
                        uint8_t paletteNumber : 1;
                        uint8_t xFlip : 1;
                        uint8_t yFlip : 1;
                        uint8_t renderPriority : 1;
                    };
                    uint8_t value;
                };
            } options;
        }sprites[40] = {Sprite()};

        struct Tile {
            uint8_t  pixels[8][8] = {0};
        }tiles[384];

        const Colour palette_colours[4] = {
                { 255, 255, 255, 255},
                { 192,192,192,255},
                { 96,96,96, 255 },
                { 0, 0, 0, 255 },
        };

        Colour palette_BGP[4] = {
                { 255, 255, 255, 255},
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
        };

        Colour palette_OBP0[4] = {
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
        };

        Colour palette_OBP1[4] = {
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
        };

        bool romDisabled = false;
        bool is_halted = false;

        void load_boot_rom(std::string location);
        void load_cartrige_rom(std::string location);

        void write_byte(uint16_t address, uint8_t value);
        uint8_t read_byte(uint16_t address);

        void write_short(uint16_t address, uint16_t value);
        uint16_t read_short(uint16_t address);

        void write_short_stack(uint16_t *sp, uint16_t value);
        uint16_t read_short_stack(uint16_t *sp);

        void updateTile(uint16_t address, uint8_t value);
        void updateSprite(uint16_t address, uint8_t value);
};

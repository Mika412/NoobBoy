#pragma once

#include <stdint.h>
#include <fstream>

#include "Registers.h"

struct rgb {
    unsigned char r, g, b, a;
};
typedef struct rgb COLOUR;

class MMU {
    private:

    public:
        // TODO: MAKE THIS SOMEHOW PRIVATE
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
        
        struct renderFlag {
            bool viewport = false;
            bool tiles = false;
            bool background = false;
        } renderFlag;

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

        uint8_t joypad = 0xFF;

        struct sprite {
            unsigned char y;
            unsigned char x;
            unsigned char tile;
            struct spriteOptions {
                uint8_t renderPriority : 1;
                uint8_t yFlip : 1;
                uint8_t xFlip : 1;
                uint8_t palleteNumber : 1;
                uint8_t gbcVRAMBank: 1;
                uint8_t gbcPalleteNumber3 : 1;
                uint8_t gbcPalleteNumber2 : 1;
                uint8_t gbcPalleteNumber1 : 1;
            } options;
        };

        const COLOUR palette_colours[4] = {
                { 255, 255, 255, 255},
                // { 0, 192, 192, 192 },
                // { 0, 96, 96, 96 },
                // { 0, 255, 255, 255 },
                // { 0, 255, 255, 255 },
                { 192,192,192,255},
                { 96,96,96, 255 },
                { 0, 0, 0, 255 },
        };

        COLOUR palette_BGP[4] = {
                { 255, 255, 255, 255},
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
        };

        COLOUR palette_OBP0[4] = {
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
        };

        COLOUR palette_OBP1[4] = {
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
                { 0, 0, 0, 255 },
        };

        uint8_t tiles[384][8][8] = {0}; 
        sprite sprites[40] = {sprite()}; 

        bool updatedTiles = false;
        
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

        void updateTile(unsigned short address, unsigned char value);
        void updateSprite(unsigned short address, unsigned char value);
        void updateColourPalette(COLOUR palette, unsigned char value);
};

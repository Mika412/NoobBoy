#pragma once

#include <stdint.h>
#include <fstream>

#include "Registers.h"

class MMU {
    private:

    public:
        // TODO: MAKE THIS SOMEHOW PRIVATE
        uint8_t memory[0xFFFF];

        uint8_t ROMbanks[0x7F][0x4000];
        uint8_t banksLoaded;
        
        struct renderFlag {
            bool viewport = false;
            bool tiles = false;
            bool background = false;
        } renderFlag;

        struct clock {
            unsigned char m = 0;
            unsigned char t = 0;
        } clock;


        struct timer{
            unsigned char div = 0;
            unsigned char tima = 0;
            unsigned char tma = 0;
            unsigned char tac = 0;
        } timer;

        uint8_t joypad = 0xFF;
        // uint16_t joypad = 0x0F0F;
        // unsigned char column = 0;
        

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

        uint8_t tiles[384][8][8] = {0}; 
        sprite sprites[40] = {sprite()}; 
        uint8_t sprites_y_cord[40] = {0}; 
        uint8_t sprites_x_cord[40] = {0}; 

        bool updatedTiles = false;
        
        bool romDisabled = false;

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
};

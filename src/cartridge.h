#pragma once

#include "mbc.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <sstream>

class Cartridge {
    public:
        char rom_title[16];
		bool cgb_game = false;
    
        MBC *mbc;

        uint8_t memory[0x7F * 0x4000];
        uint8_t ram[0x7F * 0x2000];

        Cartridge(std::string rom, std::string save_file = "");

        void load_game_rom(std::string location);
        void detect_mbc_type(uint8_t type);

        uint8_t mbc_read(uint16_t address){ return mbc->read_byte(address); };
        void mbc_write(uint16_t address, uint8_t value) { mbc->write_byte(address, value); };

        void load_save_state(std::string save_file);
        void write_save_state();
};

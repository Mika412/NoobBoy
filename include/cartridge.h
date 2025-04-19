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
    std::string rom_title;
    bool cgb_game = false;
    uint8_t mbc_type = 0x0;

    MBC *mbc;

    uint8_t *memory;
    uint8_t *ram;

    int rom_banks_count;
    int ram_banks_count;

    Cartridge(std::string rom, std::string save_file = "");

    void load_game_rom(std::string location);
    void detect_mbc_type(uint8_t type);
    int get_ram_banks_count(uint8_t type);

    uint8_t mbc_read(uint16_t address) { return mbc->read_byte(address); };
    void mbc_write(uint16_t address, uint8_t value) { mbc->write_byte(address, value); };

    void load_save_state(std::string save_file);
    void write_save_state();

    void printInfo();
};

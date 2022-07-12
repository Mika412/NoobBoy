#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>

class Cartridge {
    public:
        char rom_title[16];
		bool cgb_game = false;

        bool mbcRamEnabled = false;
        uint8_t mbcRomMode = 0;
        uint8_t mbcRomNumber = 1;
        uint8_t mbcRamNumber = 0;


        uint8_t ROMbanks[0x7F][0x4000];
        uint8_t RAMbanks[0x7F][0x2000];

        Cartridge(std::string rom, std::string save_file = "");

        uint8_t mbc_rom_read(uint16_t address);
        void mbc_rom_write(uint16_t address, uint8_t value);

        uint8_t mbc_ram_read(uint16_t address);
        void mbc_ram_write(uint16_t address, uint8_t value);

        void load_game_rom(std::string location);
        void load_save_state(std::string save_file);
        void write_save_state();
};

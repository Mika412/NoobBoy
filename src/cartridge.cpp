#include "cartridge.h"

Cartridge::Cartridge(std::string rom, std::string save_file) {
    load_game_rom(rom);
	load_save_state(save_file);
}

void Cartridge::load_game_rom(std::string location) {
    std::ifstream GAME_ROM(location, std::ios::binary);
    GAME_ROM.seekg(0, std::ios::end);
    long size = GAME_ROM.tellg();
    if (size % (16 * 1024) != 0) {
        std::cout << "Size must be a multiple of 16 KB" << std::endl;
        return;
    }

    GAME_ROM.seekg(0, std::ios::beg);
    for (long i = 0; i < size / 0x4000; i++) {
        GAME_ROM.seekg(i * 0x4000);
        GAME_ROM.read((char*)ROMbanks[i], 0x4000);
    }
    
    std::copy(ROMbanks[0] + 0x134, ROMbanks[0] + 0x143, rom_title);
    cgb_game = ROMbanks[0][0x143] == 0x80 || ROMbanks[0][0x143] == 0xC0;

    std::cout << "Rom Title: " << +rom_title << std::endl;
    std::cout << "CGB Game : " << +cgb_game << std::endl;
}

void Cartridge::load_save_state(std::string save_file){
    if(save_file.empty())
		return;

    std::ifstream SAVE(save_file, std::ios::binary);
    SAVE.seekg(0, std::ios::end);

    long size = SAVE.tellg();
    if(size != (0x7f * 0x2000 + sizeof(rom_title))){
        std::cout << "Save file possibly corrupted. Save not loaded." << std::endl;
        return;
    }
    char save_title[16];
    SAVE.seekg(0, std::ios::beg);
    SAVE.read((char*)save_title, sizeof(save_title));
    std::cout << "Save file " << save_title << std::endl;
    if(strcmp(rom_title, save_title) != 0 ){
        std::cout << "This save file is not for this rom. Save not loaded." << std::endl;
        return;
    }
    SAVE.seekg(sizeof(save_title));
    SAVE.read((char*)RAMbanks, 0x7f * 0x2000);

    std::cout << "Save file loaded successfully" << std::endl;
}

void Cartridge::write_save_state(){
    std::filesystem::create_directory("saves");

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << "saves/" 
        << reinterpret_cast<char *>(rom_title) 
        << "_" 
        << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") 
        << ".save";
    std::string filename = oss.str();

    std::ofstream out(filename, std::ios_base::binary);
    out.write((char*)rom_title, sizeof(rom_title));
    out.write((char*)RAMbanks, sizeof(uint8_t)*(0x7f * 0x2000));

    std::cout << "Saved state to: " << filename << std::endl;
}

uint8_t Cartridge::mbc_rom_read(uint16_t address) {
    if (address < 0x4000)
        return ROMbanks[0][address];

	return ROMbanks[mbcRomNumber][address - 0x4000];
}

void Cartridge::mbc_rom_write(uint16_t address, uint8_t value) {
    if (address < 0x2000)
        mbcRamEnabled = value > 0;
    else if (address >= 0x2000 && address < 0x4000) 
        mbcRomNumber  = ((value ? value & 63 : 1));
    else if (address < 0x6000) 
        mbcRamNumber = value & 3;
    // else if (address >= 0x6000 && address < 0x8000) {
    //     mbcRomMode = value > 0;
    // }
}

uint8_t Cartridge::mbc_ram_read(uint16_t address) {
	return RAMbanks[mbcRamNumber][address];
}

void Cartridge::mbc_ram_write(uint16_t address, uint8_t value) {
	if(mbcRamEnabled)
		RAMbanks[this->mbcRamNumber][address] = value;
}

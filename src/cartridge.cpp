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
    
    GAME_ROM.seekg(std::ios::beg);
    GAME_ROM.read((char*)memory, size);

    std::copy(memory + 0x134, memory + 0x143, rom_title);
    cgb_game = memory[0x143] == 0x80 || memory[0x143] == 0xC0;
    detect_mbc_type(memory[0x147]);

    std::cout << "Rom Title: " << +rom_title << std::endl;
    std::cout << "CGB Game : " << +cgb_game << std::endl;
    std::cout << "MBC: " << +memory[0x147] << std::endl;

}

void Cartridge::detect_mbc_type(uint8_t type) {
    switch (type) {
        case 0x00:
        case 0x08:
        case 0x09:
            mbc = new MBC0(memory);
            break;
        case 0x01:
        case 0x02:
        case 0x03:
            mbc = new MBC1(memory, ram);
            break;
        case 0x05:
        case 0x06:
            mbc = new MBC2(memory, ram);
            break;
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            mbc = new MBC3(memory, ram);
            break;
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            mbc = new MBC5(memory, ram);
            break;
        default:
			std::cout << "Unsupported MBC type: " << type << std::endl;
            exit(1);
    }
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
    SAVE.read((char*)ram, 0x7f * 0x2000);

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
    out.write((char*)ram, sizeof(uint8_t)*(0x7f * 0x2000));

    std::cout << "Saved state to: " << filename << std::endl;
}

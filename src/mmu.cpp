#include "mmu.h"
#include "ppu.h"

MMU::MMU(Cartridge *cartridge) {
	this->cartridge = cartridge;
}

void MMU::load_boot_rom(std::string location) {
    std::ifstream DMG_ROM(location, std::ios::binary);
    DMG_ROM.seekg(0, std::ios::end);
    long size = DMG_ROM.tellg();
    DMG_ROM.seekg(0, std::ios::beg);
    DMG_ROM.read((char*)memory, 0x100);
}


void MMU::save_game_state(){
	cartridge->write_save_state();
}

void MMU::UpdateTile(uint16_t laddress, uint8_t value) {
    uint16_t address = laddress & 0xFFFE;
    
    uint16_t tile = (address >> 4) & 511;
    uint16_t y = (address >> 1) & 7;

    uint8_t bitIndex;
    for(uint8_t x = 0; x < 8; x++) {
        bitIndex = 1 << (7 - x);

        tiles[tile].pixels[y][x] = ((memory[address] & bitIndex) ? 1 : 0) + ((memory[address + 1] & bitIndex) ? 2 : 0);
    }
}

void MMU::UpdateSprite(uint16_t laddress, uint8_t value) {
    uint16_t address = laddress - 0xFE00;
    Sprite *sprite = &sprites[address >> 2];
    sprite->ready = false;
    switch(address & 3){
        case 0:
            sprite->y = value - 16;
            break;
        case 1:
            sprite->x = value - 8;
            break;
        case 2:
            sprite->tile = value;
            break;
        case 3:
            sprite->options.value = value;
            sprite->colourPalette = (sprite->options.paletteNumber) ? palette_OBP1 : palette_OBP0;
            sprite->ready = true;
            break;
    }
}

void MMU::UpdatePalette(Colour *palette, uint8_t value) {
    palette[0] = palette_colours[value & 0x3];
    palette[1] = palette_colours[(value >> 2) & 0x3];
    palette[2] = palette_colours[(value >> 4) & 0x3];
    palette[3] = palette_colours[(value >> 6) & 0x3];

}

uint8_t MMU::read_byte(uint16_t address) {
    if(address == 0xff00) {
        switch(memory[0xff00] & 0x30){ // Mask `00110000` to check which SELECT
            case 0x10: return (uint8_t)(joypad & 0x0F) | 0x10; 
            case 0x20: return (uint8_t)(joypad >> 4) & 0x0F | 0x20; 
            default: return 0xFF;
        }
    }

    //Timers
    else if(address == 0xff04) return timer.div;
    else if(address == 0xff05) return timer.tima;
    else if(address == 0xff06) return timer.tma;
    else if(address == 0xff07) return timer.tac;
   
    if(address == 0xff0f) return memory[0xFF0F];

    if (address < 0x100 && !romDisabled) return memory[address];
    
    // Switchable ROM banks
    if (address < 0x8000)
        return cartridge->mbc_read(address);
    
    // Switchable RAM banks
    if (address >= 0xA000 && address <= 0xBFFF)
		return cartridge->mbc_read(address);
    
    return memory[address];
}

void MMU::write_byte(uint16_t address, uint8_t value) {
    if(address == 0xFF40){
        memory[address] =  value;
        if (!(value & (1 << 7))){
            memory[0xFF44] = 0x00;
            memory[0xFF41] &= 0x7C;
        }
    }

    if(address >= 0xFEA0 && address <= 0xFEFF) // Writing in unused area
        return;

    // Copy Sprites from ROM to RAM (OAM)
    if(address == 0xFF46)
        for(uint16_t i = 0; i < 160; i++) write_byte(0xFE00 + i, read_byte((value << 8) + i));


    if(address == 0xff50)
        romDisabled = true;
    
    //Timers
    else if(address == 0xff04) timer.div = 0;
    else if(address == 0xff05) timer.tima = value;
    else if(address == 0xff06) timer.tma = value;
    else if(address == 0xff07) timer.tac = value;
    
    // Update colour palette
    else if(address == 0xff47) UpdatePalette(palette_BGP, value);
    else if(address == 0xff48) UpdatePalette(palette_OBP0, value);               
    else if(address == 0xff49) UpdatePalette(palette_OBP1, value);
    
    // Switchable ROM banks
    if (address < 0x8000)
		cartridge->mbc_write(address, value);
	else if (address >= 0xA000 && address < 0xC000)
		cartridge->mbc_write(address, value);
    else
        memory[address] = value;


    if(address >= 0x8000 && address < 0x9800)
        UpdateTile(address, value);

    if(address >= 0xFE00 && address <= 0xFE9F)
        UpdateSprite(address, value);


}

uint16_t MMU::read_short(uint16_t address){
    return read_byte(address) | (read_byte(address + 1) << 8);
}

void MMU::write_short(uint16_t address, uint16_t value) {
    write_byte(address, (uint8_t)(value & 0x00ff));
    write_byte(address + 1, (uint8_t)((value & 0xff00) >> 8)); 
}

void MMU::write_short_stack(uint16_t *sp, uint16_t value) {
    *sp -= 2;
    write_short(*sp, value);
}

uint16_t MMU::read_short_stack(uint16_t *sp) {
    uint16_t value = read_short(*sp);
    *sp += 2;  

    return value;
}



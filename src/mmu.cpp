#include "mmu.h"
#include "keys.h"
#include "ppu.h"

using namespace std;

void MMU::load_boot_rom(string location) {
    ifstream DMG_ROM(location, ios::binary);
    DMG_ROM.seekg(0, ios::end);
    long size = DMG_ROM.tellg();
    DMG_ROM.seekg(0, std::ios::beg);
    DMG_ROM.read((char*)memory, 0x100);
}


void MMU::load_cartrige_rom(string location) {
    ifstream GAME_ROM(location, ios::binary);
    GAME_ROM.seekg(0, ios::end);
    long size = GAME_ROM.tellg();
    if (size % (16 * 1024) != 0) {
        cout << "Size must be a multiple of 16 KB" << endl;
        return;
    }

    GAME_ROM.seekg(0, ios::beg);
    for (long i = 0; i < size / 0x4000; i++) {
        // cout << "LOADED ROM " << +banksLoaded << endl;
        GAME_ROM.seekg(i * 0x4000);
        GAME_ROM.read((char*)ROMbanks[i], 0x4000);
        banksLoaded++;
    }
    switch (ROMbanks[0][0x147]){
        case 1 : this->mbcType = 1; break;
        case 2 : this->mbcType = 1; break;
        case 3 : this->mbcType = 1; break;
        case 5 : this->mbcType = 2; break;
        case 6 : this->mbcType = 2; break;
        default : break;
    }

    // std::cout << "MBC TYPE " << std::hex << +mbcType << std::endl; 
    // std::cout << "ROM TYPE " << std::hex << +ROMbanks[0][0x147] << std::endl; 
    // exit(1);
}

void MMU::updateTile(uint16_t laddress, uint8_t value) {
    uint16_t address = laddress & 0xFFFE;
    
    uint16_t tile = (address >> 4) & 511;
    uint16_t y = (address >> 1) & 7;

    unsigned char bitIndex;
    for(uint8_t x = 0; x < 8; x++) {
        bitIndex = 1 << (7 - x);

        tiles[tile].pixels[y][x] = ((memory[address] & bitIndex) ? 1 : 0) + ((memory[address + 1] & bitIndex) ? 2 : 0);
    }
}

void MMU::updateSprite(uint16_t laddress, uint8_t value) {
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

uint8_t MMU::read_byte(uint16_t address) {
    if(address == 0xff00) {
        switch(memory[0xff00] & 0x30){ // Mask `00110000` to check which SELECT
            case 0x10: return (uint8_t)(joypad & 0x0F) | 0x10; 
            case 0x20: return (uint8_t)(joypad >> 4) & 0x0F | 0x20; 
            default: return 0xFF;
        }
    }
    // if(address >= 0xFEA0 && address <= 0xFEFF)
    //     exit(1);

    //Timers
    else if(address == 0xff04) return timer.div;
    else if(address == 0xff05) return timer.tima;
    else if(address == 0xff06) return timer.tma;
    else if(address == 0xff07) return timer.tac;
   
    if(address == 0xff0f) return memory[0xFF0F];

    if (address < 0x100 && !romDisabled) return memory[address];
    
    // Switchable ROM banks
    if (address < 0x4000)
        return ROMbanks[0][address];
    else if(address >= 0x4000 && address < 0x8000)
        return ROMbanks[mbcRomNumber][address - 0x4000];
    
    // Switchable RAM banks
    if (address >= 0xA000 && address <= 0xBFFF){
        return RAMbanks[mbcRamNumber][address - 0xA000];
    }

    
    if(address >= 0xFE00 && address < 0xFE9F) return 0; 

    // if(address >= 0xA000 && address < 0xBFFF){
    //     return RAMbanks[mbc1ramNumber][address - 0xA000];
    // }
    
    return memory[address];
}

void MMU::write_byte(uint16_t address, uint8_t value) {
    if(address >= 0xFEA0 && address <= 0xFEFF) // Writing in unused area. TODO: Prettify this
        return;
    
    // Copy Sprites from ROM to RAM (OAM)
    if(address == 0xFF46){ 
        for(uint16_t i = 0; i < 160; i++) write_byte(0xFE00 + i, read_byte((value << 8) + i));
    }

    if(address == 0xff45){
        std::cout << "LYC: " << +value << std::endl;
        std::cout << "LY: " << +memory[0xff44] << std::endl;
        std::cout << "STAT: " << +memory[0xff41] << std::endl;
    }

    if(address == 0xff50){
        romDisabled = true;
    }
    //Timers
    else if(address == 0xff04) timer.div = 0;
    else if(address == 0xff05) timer.tima = value;
    else if(address == 0xff06) timer.tma = value;
    else if(address == 0xff07){
        timer.tac = value;
        // timer.tac = value & 7;
    }
    
    // TODO: CLEAN THIS UP
    // Update colour palette
    else if(address == 0xff47){
        palette_BGP[0] = palette_colours[value & 0x3];
        palette_BGP[1] = palette_colours[(value >> 2) & 0x3];
        palette_BGP[2] = palette_colours[(value >> 4) & 0x3];
        palette_BGP[3] = palette_colours[(value >> 6) & 0x3];
    }               
    else if(address == 0xff48){
        palette_OBP0[0] = palette_colours[value & 0x3];
        palette_OBP0[1] = palette_colours[(value >> 2) & 0x3];
        palette_OBP0[2] = palette_colours[(value >> 4) & 0x3];
        palette_OBP0[3] = palette_colours[(value >> 6) & 0x3];
    }               
    else if(address == 0xff49){
        palette_OBP1[0] = palette_colours[value & 0x3];
        palette_OBP1[1] = palette_colours[(value >> 2) & 0x3];
        palette_OBP1[2] = palette_colours[(value >> 4) & 0x3];
        palette_OBP1[3] = palette_colours[(value >> 6) & 0x3];
    }               
    if(address == 0xff0f){
        memory[0xFF0F] = value;// & 0x1F;
        return;
    }
    // if(address >= 0x8000)
    //     memory[address] = value;

     //	MBC0
    // if (this->mbcType == 0x00) {
            
    //     //	make ROM readonly
    //     if (address >= 0x8000)
    //         memory[address] = value;
    // }
    //	MBC1
    if (true) {
    // if (this->mbcType == 0x00) {
    // if (this->mbcType == 0x01) {

        //	external RAM enable / disable
        if (address < 0x2000)
            mbcRamEnabled = value > 0;

        //	choose ROM bank nr (lower 5 bits, 0-4)
        if (address >= 0x2000 && address < 0x4000) {
            if((value & 0x1f) > 64){
                std::cout << "YO: " << std::dec << +(value & 0x1f) << std::endl;
                return;
            }
            mbcRomNumber = value & 0x1f;
            // if((value & 0x1f) > 1 && (value & 0x1f) != 12){
            //     std::cout << "HHE: " << std::dec << +(value & 0x1f) << std::endl;
            //     exit(1);
            // }
            if (value == 0x00 || value == 0x20 || value == 0x40 || value == 0x60)
                mbcRomNumber = (value & 0x1f) + 1;

            // std::cout << "MBC ROM NUMBER: " << std::hex << +mbc1romNumber << std::endl;
        }
        // //	choose RAM bank nr OR ROM bank top 2 bits (5-6)
        else if (address < 0x6000) {
            // //	mode: ROM bank 2 bits
            // if (mbcRomMode == 0)
            //     mbcRomNumber |= (value & 3) << 5;
            // //	mode: RAM bank selection
                mbcRamNumber = value & 3;
        }
        // else if (address >= 0x6000 && address < 0x8000) {
        //     mbcRomMode = value > 0;
        // }
        else {
            memory[address] = value;
        }
    }   

    if(address >= 0xA000 && address < 0xC000){
        if(this->mbcRamEnabled){
            this->RAMbanks[this->mbcRamNumber][address - 0xA000] = value;
        }
    }

    if(address >= 0x8000 && address <= 0x9fff) {
        if(address <= 0x97ff){
            updateTile(address, value);
        }
    }

    if(address >= 0xFE00 && address <= 0xFE9F) {
        updateSprite(address, value);
    }


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



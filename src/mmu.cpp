#include <cstdint>
#include <fstream>
#include <cstring>
#include <iostream>
#include "mmu.h"
#include "keys.h"

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
}

void MMU::updateTile(unsigned short laddress, unsigned char value) {
    uint16_t address = laddress & 0xFFFE;
    // address &= 0x1ffe;
    
    uint16_t tile = (address >> 4) & 511;
    uint16_t y = (address >> 1) & 7;
    
    unsigned char bitIndex;
    for(uint8_t x = 0; x < 8; x++) {
            bitIndex = 1 << (7 - x);
        
        //((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
        // tiles[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
        tiles[tile][y][x] = ((memory[address] & bitIndex) ? 1 : 0) + ((memory[address + 1] & bitIndex) ? 2 : 0);
    }

    renderFlag.tiles = true;
}
// void MMU::updateTile(uint16_t address, uint8_t lower) {
//     // if(address & 1)
//     //     return;
//     address &= 0xFFFE;

//     uint8_t upper = this->read_byte(address + 1);

//     int tmp = (address & 0x1fff);
//     int tile_id = tmp >> 4;
//     int y = (tmp >> 1) & 0x7;

//     for(int i = 0; i < 8; i++)
//         this->tiles[tile_id][y][7 - i] = ((upper >> i) & 1) | (((lower >> i) & 1) << 1);
// }

void MMU::updateSprite(unsigned short laddress, uint8_t value) {
    // if((laddress & 3) != 2){
    //     return;
    // }
    std::cout << "ADDRESS " << std::hex << +laddress << " SPRITE "<< std::hex << +value << std::endl;
    uint16_t address = laddress - 0xFE00;
    // uint16_t address = laddress & 0xFFFE;
    unsigned short tile = (address >> 2);
    sprite *spr = &sprites[tile];
    // std::cout << "SP:RITE " << &sprites[tile] << std::endl;
    printf("TILE %d\n", tile); 
    switch(address & 3){
        case 0:
            spr->y = value - 16;
            break;
        case 1:
            spr->x = value - 8;
            break;
        case 2:
            spr->tile = value;
            break;
        case 3:
            // TODO: Rewrite this as a struct cast
            // struct spriteOptions *sprs = (spriteOptions *)value;
            spr->options.gbcPalleteNumber1  = value >> 0;
            spr->options.gbcPalleteNumber2  = value >> 1;
            spr->options.gbcPalleteNumber3  = value >> 2;
            spr->options.gbcVRAMBank        = value >> 3;
            spr->options.palleteNumber      = value >> 4;
            spr->options.xFlip              = value >> 5;
            spr->options.yFlip              = value >> 6;
            spr->options.renderPriority     = value >> 7;
            // spr->options = (struct sprite.options *)value;
            break;
    //         // address &= 0x1ffe;
            
    //         unsigned short y = (address >> 1) & 7;
            
    //         unsigned char bitIndex;
    //         for(unsigned char x = 0; x < 8; x++) {
    //                 bitIndex = 1 << (7 - x);
                
    //             //((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
    //             // tiles[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
    //             sprites[tile][y][x] = ((memory[address] & bitIndex) ? 1 : 0) + ((memory[address + 1] & bitIndex) ? 2 : 0);
    //         }
    // }
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
    if(address >= 0xFEA0 && address <= 0xFEFF)
        exit(1);

    //Timers
    // else if(address == 0xff04) return timer.div;
    else if(address == 0xff04) return (unsigned char) rand();
    else if(address == 0xff05) return timer.tima;
    else if(address == 0xff06) return timer.tma;
    else if(address == 0xff07) return timer.tac;

    if (address < 0x100 && !romDisabled) {
        return memory[address];
    } 
    if (address < 0x4000) {
        return ROMbanks[0][address];
    } else if (address < 0x8000) {
        return ROMbanks[1 % banksLoaded][address - 0x4000];
    }
    // if(address >= 0xFE00 && address < 0xFE9F) {
    //     std::cout << "Entered" <<  std::endl;
    // }
    return memory[address];
}

void MMU::write_byte(uint16_t address, uint8_t value) {
    if(address >= 0xFEA0 && address <= 0xFEFF) // Writing in unused area. TODO: Prettify this
        return;
    
    // Copy Sprites from ROM to RAM (OAM)
    if(address == 0xFF46){ 
        // std::cout << "\nFUCK "<< +(value << 8) << std::endl;
        // exit(1);
        // for(uint16_t i = 0; i < 20; i++) write_byte(0xFE00 + i, read_byte((value << 8) + i));
        for(uint16_t i = 0; i < 160; i++) write_byte(0xFE00 + i, read_byte((value << 8) + i));
    }
    // if(address == 0xff00){
    //     keys.column = value & 0x30;
    // }
    if(address == 0xff50){
        romDisabled = true;
    }
    //Timers
    else if(address == 0xff04) timer.div = 0;
    else if(address == 0xff05) timer.tima = value;
    else if(address == 0xff06) timer.tma = value;
    else if(address == 0xff07) timer.tac = value & 7;

    memory[address] = value;
    if(address >= 0x8000 && address <= 0x9fff) {
        if(address <= 0x97ff){
            updateTile(address, value);
        }else{
            renderFlag.background = true;
        }
    }
    // if(address == 0xC002) {
    //     printf("TILE %x\n", +value);
    // }
    if(address >= 0xFE00 && address < 0xFE9F) {
        // if(value > 0){
        //     exit(1);
        // }
        renderFlag.background = true;
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

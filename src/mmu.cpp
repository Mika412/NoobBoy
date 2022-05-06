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
    
    unsigned short tile = (address >> 4) & 511;
    unsigned short y = (address >> 1) & 7;
    
    unsigned char bitIndex;
    for(unsigned char x = 0; x < 8; x++) {
            bitIndex = 1 << (7 - x);
        
        //((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
        // tiles[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
        tiles[tile][y][x] = ((memory[address] & bitIndex) ? 1 : 0) + ((memory[address + 1] & bitIndex) ? 2 : 0);
    }

    renderFlag.tiles = true;
}

uint8_t MMU::read_byte(uint16_t address) {
    if(address == 0xff00) {
        switch(keys.column){
            case 0x10: return keys.keys1;
            case 0x20: return keys.keys2;
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
    return memory[address];
}

void MMU::write_byte(uint16_t address, uint8_t value) {
    if(address >= 0xFEA0 && address <= 0xFEFF) // Writing in unused area. TODO: Prettify this
        return;

    if(address == 0xff00){
        keys.column = value & 0x30;
    }
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

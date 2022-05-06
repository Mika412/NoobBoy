#include <cstdint>
#include <fstream>
#include <cstring>
#include <iostream>
#include "Memory.h"
#include "keys.h"

using namespace std;

MemoryBus::MemoryBus(Registers *registers){
    this->registers = registers;
    // cout << "TESTING" << endl;
    memcpy(io, ioReset, sizeof(io));                        
    memset(hram,0,sizeof(hram));
    memset(vram,0,sizeof(vram));
    memset(memory,0,sizeof(memory));
    memset(cart,0,sizeof(cart));

    for(int i = 0; i < 384; i++)
    {
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                tiles[i][y][x] = 0;
            }
        }
    }

}

void MemoryBus::load_boot_rom(string location) {
    ifstream DMG_ROM(location, ios::binary);
    DMG_ROM.seekg(0, ios::end);
    long size = DMG_ROM.tellg();
    DMG_ROM.seekg(0, std::ios::beg);
    DMG_ROM.read((char*)memory, 0x100);
}


void MemoryBus::load_cartrige_rom(string location) {
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

void MemoryBus::updateTile(unsigned short address, unsigned char value) {
        address &= 0x1ffe;
        
        unsigned short tile = (address >> 4) & 511;
        unsigned short y = (address >> 1) & 7;
        
        unsigned char bitIndex;
        for(unsigned char x = 0; x < 8; x++) {
                bitIndex = 1 << (7 - x);
            
            //((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
            tiles[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
        }

        renderFlag.tiles = true;
    }

uint8_t MemoryBus::read_byte(uint16_t address) {
    if(address == 0xff00) {
        switch(keys.column){
            case 0x10: return keys.keys1;
            case 0x20: return keys.keys2;
            default: return 0;
        }
    }
    if(address >= 0xFEA0 && address <= 0xFEFF)
        exit(1);

    if(address >= 0x8000 && address <= 0x9fff)
        return vram[address - 0x8000];
    if(address == 0xff42) return gpu.scrollY;
    else if(address == 0xff40) return gpu.control;
    else if(address == 0xff43) return gpu.scrollX;
    else if(address == 0xff44) return gpu.scanline;
    // else if(address == 0xff0f) return interruptFlags.IF;
    // else if(address == 0xffff) return interruptFlags.IE;
    //Timers
    // else if(address == 0xff04) return timer.div;
    else if(address == 0xff04) return (unsigned char) rand();
    else if(address == 0xff05) return timer.tima;
    else if(address == 0xff06) return timer.tma;
    else if(address == 0xff07) return timer.tac;

    // else if(address == 0xff85) return 0; // TODO: REMOVE THIS
    // if(address == 0xff00) {
    //     return 0xef;
    //     if(!(io[0x00] & 0x20)) {
                // return (unsigned char)(0xc0 | 0x10);
    //             return (unsigned char)(0xc0 | 4 | 0x10);
    //     }        
    //     else if(!(io[0x00] & 0x10)) {
                // return (unsigned char)(0xc0 | 0x20);
    //             return (unsigned char)(0xc0 | 4 | 0x20);
    //     }        
    //     else if(!(io[0x00] & 0x30)) return 0xff;
    //     else return 0;
    // }
    address &= 0xFFFF;
    if (address >= 0xE000 && address < 0xFE00) {
        address -= 0x2000;
    }
    if (address < 0x100 && !romDisabled) {
    // if (address < 0x100 && (memory[0xFF50] & 0x01) != 0x01) {
        return memory[address];
    } 
    if (address < 0x4000) {
        return ROMbanks[0][address];
    } else if (address < 0x8000) {
        return ROMbanks[1 % banksLoaded][address - 0x4000];
    }
    /* else if(address >= 0xff00 && address <= 0xff7f){
        // std::cout << "ENTERED IO " << std::hex << +(address - 0xff00)  << " VALUE " << std::hex << +io[address - 0xff00] << std::endl;
        if(address == 0xff00){ // TODO: Temp fix
            return 0xFF;
        }
        return io[address - 0xff00];
    } */
    else if(address >= 0xff80 && address <= 0xfffe)
            return hram[address - 0xff80];  
    return memory[address];
}

void MemoryBus::write_byte(uint16_t address, uint8_t value) {
    if(address >= 0xFEA0 && address <= 0xFEFF) // Writing in unused area. TODO: Prettify this
        return;

    if(address == 0xff00){
        keys.column = value & 0x30;
    }
    if(address == 0xff50){
        romDisabled = true;
    }
    if(address == 0xff42) gpu.scrollY = value;
    else if(address == 0xff40) { // Screen enable
        gpu.control = value;
    }
    // else if(address == 0xff0f)  interruptFlags.IF = value;
    // else if(address == 0xffff)  interruptFlags.IE = value;
    //Timers
    else if(address == 0xff04) timer.div = 0;
    else if(address == 0xff05) timer.tima = value;
    else if(address == 0xff06) timer.tma = value;
    else if(address == 0xff07) timer.tac = value & 7;

    else if(address == 0xff43) gpu.scrollX = value;
    if(address >= 0x8000 && address <= 0x9fff) {
        // std::cout << "WROTE " << hex << +value << " TO " <<hex << +address<< std::endl;
        vram[address - 0x8000] = value;
        if(address <= 0x97ff){
            updateTile(address, value);
        }else{
            renderFlag.background = true;
        }
    /* } else if(address >= 0xff00 && address <= 0xff7f){

        io[address - 0xff00] = value; */
    }
    else if(address >= 0xff80 && address <= 0xfffe)
        hram[address - 0xff80] = value;
    else{
        memory[address] = value;
    }
}

uint16_t MemoryBus::read_short(uint16_t address){
    return read_byte(address) | (read_byte(address + 1) << 8);
}

void MemoryBus::write_short(uint16_t address, uint16_t value) {
    write_byte(address, (uint8_t)(value & 0x00ff));
    write_byte(address + 1, (uint8_t)((value & 0xff00) >> 8)); 
}

void MemoryBus::write_short_stack(uint16_t value) {
    registers->sp -= 2;
    write_short(registers->sp, value);
}

uint16_t MemoryBus::read_short_stack() {
    uint16_t value = read_short(registers->sp);
    registers->sp += 2;  

    return value;
}

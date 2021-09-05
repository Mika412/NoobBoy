#include "interrupt.h"
#include <iostream>

Interrupt::Interrupt(Registers *registers, MemoryBus *memory){
    this->registers = registers;
    this->memory = memory;
}

bool Interrupt::step(){
    if(memory->interruptFlags.IME && memory->interruptFlags.IE && memory->interruptFlags.IF){
        uint8_t fire = memory->interruptFlags.IE & memory->interruptFlags.IF;

        if(fire & (1 << 0)){ // VBLANK
            memory->interruptFlags.IF &= ~(1 << 0);
            memory->interruptFlags.IME = 0;
            memory->write_short_stack(registers->pc);
            registers->pc = 0x40;
            registers->t += 12;
            return true;
        }
        if(fire & (1 << 2)){ // TIMER
            memory->interruptFlags.IF &= ~(1 << 2);
            memory->interruptFlags.IME = 0;
            memory->write_short_stack(registers->pc);
            registers->pc = 0x50;
            registers->t += 12;
            return true;
        }
        if(fire & (1 << 4)){ // JOYPAD
            std::cout << "SHOULD JOYPAD" << std::endl;
            memory->interruptFlags.IF &= ~(1 << 4);
            memory->interruptFlags.IME = 0;
            memory->write_short_stack(registers->pc);
            registers->pc = 0x60;
            registers->t += 12;
            return true;
        }
        // exit(1);
    }
        // std::cout << "Enabled IME " << std::hex << +memory->interruptFlags.IME << std::endl;
        // std::cout << "Enabled IE " << std::hex << +memory->interruptFlags.IE << std::endl;
        // std::cout << "Enabled IF " << std::hex << +memory->interruptFlags.IF << std::endl;
    return false;
}

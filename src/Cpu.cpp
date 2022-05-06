#include <iostream>

#include "cpu.h"

using namespace std;

CPU::CPU(Registers *registers, Interrupts* interrupts, MMU* memory){
    this->memory = memory;
    this->interrupts = interrupts;
    this->registers = registers;
    this->instructions = new InstructionSet(registers, interrupts, memory, &this->stopped);
    // this->instructions = new InstructionSet(registers, interrupts, memory, &this->stopped);
    /* this->registers->a = 0x01;
    this->registers->f = 0xF0;
    this->registers->b = 0x00;
    this->registers->c = 0x13;
    this->registers->d = 0x00;
    this->registers->e = 0xd8;
    this->registers->h = 0x01;
    this->registers->l = 0x4d;

    this->registers->sp = 0xfffe;
    this->registers->pc = 0x0100;

    this->registers->set_register_flag(FLAG_ZERO);
    this->registers->set_register_flag(FLAG_HALF_CARRY);
    this->registers->set_register_flag(FLAG_CARRY);
    this->registers->unset_register_flag(FLAG_SUBTRACT); */
    
    // this->memory->romDisabled = true;
    // Testing rem
    /* this->registers->a = 0x11;
    this->registers->f = 0x80;
    this->registers->b = 0x00;
    this->registers->c = 0x00;
    this->registers->d = 0xFF;
    this->registers->e = 0x56;
    this->registers->h = 0x00;
    this->registers->l = 0x0D;

    this->registers->sp = 0xfffe;
    this->registers->pc = 0x0100;

    this->registers->set_register_flag(FLAG_ZERO);
    this->registers->unset_register_flag(FLAG_HALF_CARRY);
    this->registers->unset_register_flag(FLAG_CARRY);
    this->registers->unset_register_flag(FLAG_SUBTRACT);
    
    this->memory->write_byte(0xFF00, 0xF);

    this->memory->timer.div = 0x44;
    this->memory->timer.tima = 0x00;
    this->memory->timer.tma = 0x00;
    this->memory->timer.tac = 0xF8; */

}


bool CPU::step(){
    if(stopped) return false;

    uint16_t or_pc = registers->pc;
    uint8_t instruction = this->memory->read_byte(registers->pc++);

    instructions->execute(instruction);
    
    registers->pc &= 65535; // TODO: Check if I really need this. uint16 may overflow correctly
    registers->m = registers->t / 4;
    
    // Update total clock in memory
    memory->clock.t += registers->t;
    memory->clock.m += registers->m;

    this->last_instruction = instruction;

    // return registers->pc == 0xc753; // return registers->pc == 0xC34E;
    // return registers->pc == 0x0101; // return registers->pc == 0xC34E;
    // return registers->pc == 0x0101; // return registers->pc == 0xC34E;
    return registers->pc == 0x01A4; // return registers->pc == 0xC34E;
    // return registers->pc == 0xc17e; // return registers->pc == 0xC34E;
    // return registers->pc == 0xC34A;
    // return registers->pc == 0x2817;
    // return registers->pc == 0x6a22;
    // return registers->pc == 0x2882;
    // return or_pc == 0x006b && registers.c == 1;
    // return false;
}     

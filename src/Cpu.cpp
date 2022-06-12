#include <iostream>

#include "cpu.h"

using namespace std;

CPU::CPU(Registers *registers, Interrupts* interrupts, MMU* memory){
    this->memory = memory;
    this->interrupts = interrupts;
    this->registers = registers;
    this->instructions = new InstructionSet(registers, interrupts, memory, &this->stopped);
    // this->instructions = new InstructionSet(registers, interrupts, memory, &this->stopped);


}

void CPU::no_bootrom_init() {
    this->registers->a = 0x01;
    this->registers->f = 0xb0;
    this->registers->b = 0x00;
    this->registers->c = 0x13;
    this->registers->d = 0x00;
    this->registers->e = 0xd8;
    this->registers->h = 0x01;
    this->registers->l = 0x4d;

    this->registers->sp = 0xfffe;
    this->registers->pc = 0x0100;

    this->registers->set_register_flags(FLAG_ZERO | FLAG_HALF_CARRY | FLAG_CARRY);
    this->registers->unset_register_flags(FLAG_SUBTRACT);
    // this->memory->clock.t = 324;
    this->memory->romDisabled = true;
    // this->memory->timer.div = 44032;
    // this->memory->timer.div = 0x44;
    // this->memory->timer.tac = 0xF8;
    // this->memory->write_byte(0xFF40, 91);
    this->memory->write_byte(0xFF40, 0x91);
    this->memory->memory[0xFF0F] = 0xE1;
    this->memory->write_byte(0xFF41, 0x80);
    this->memory->timer.div = 0xD3;
    this->memory->timer.tima = 0x00;
    this->memory->timer.tma = 0x00;
    this->memory->timer.tac = 0xF8;

    // // Testing rem
    // this->registers->a = 0x11;
    // this->registers->f = 0x80;
    // this->registers->b = 0x00;
    // this->registers->c = 0x00;
    // this->registers->d = 0xFF;
    // this->registers->e = 0x56;
    // this->registers->h = 0x00;
    // this->registers->l = 0x0D;

    // this->registers->sp = 0xfffe;
    // this->registers->pc = 0x0100;

    // this->registers->set_register_flags(FLAG_ZERO);
    // this->registers->unset_register_flags(FLAG_HALF_CARRY);
    // this->registers->unset_register_flags(FLAG_CARRY);
    // this->registers->unset_register_flags(FLAG_SUBTRACT);
    
    // // this->memory->write_byte(0xFF00, 0xF);

    // this->memory->romDisabled = true;
    // this->memory->timer.div = 0x44;
    // this->memory->timer.tima = 0x00;
    // this->memory->timer.tma = 0x00;
    // this->memory->timer.tac = 0xF8;
    // this->memory->write_byte(0xff45, 0);


    // for(int i = 0xFE00; i < 0xFE9F; i++){
    //     this->memory->write_byte(i, 0xFF);
    // }
}

bool CPU::step(){
    if(stopped) return false;
    if(memory->is_halted){
        this->instrs_count += 1;
        // TODO: Rewrite this
        memory->clock.t_instr = 4;
        memory->clock.t += 4;
        memory->clock.t_prev = memory->clock.t ;
        memory->clock.last_instr = 0;
        return false;
    }

    uint8_t instruction = this->memory->read_byte(registers->pc++);
    uint8_t cb_instruction = this->memory->read_byte(registers->pc);
    registers->pc &= 65535; // TODO: Check if I really need this. uint16 may overflow correctly
    this->instrs_count += 1;

    instructions->execute(instruction);
    
    
    memory->clock.t_instr = memory->clock.t - memory->clock.t_prev;
    memory->clock.t_prev = memory->clock.t;
    memory->clock.last_instr = instruction;
    if(instruction == 0xCB){
        memory->clock.last_instr = cb_instruction;
    }

    return false;
}     

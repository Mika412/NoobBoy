#include "cpu.h"

CPU::CPU(Registers *registers, Interrupts* interrupts, MMU* memory){
    this->memory = memory;
    this->interrupts = interrupts;
    this->registers = registers;
    this->instructions = new InstructionSet(registers, interrupts, memory);
}

void CPU::no_bootrom_init() {
    registers->a = 0x01;
    registers->f = 0xb0;
    registers->b = 0x00;
    registers->c = 0x13;
    registers->d = 0x00;
    registers->e = 0xd8;
    registers->h = 0x01;
    registers->l = 0x4d;

    registers->sp = 0xfffe;
    registers->pc = 0x0100;

    registers->set_flags(FLAG_ZERO | FLAG_HALF_CARRY | FLAG_CARRY, true);
    registers->set_flags(FLAG_SUBTRACT, false);

    memory->romDisabled = true;
    memory->memory[0xFF0F] = 0xE1;
    memory->write_byte(0xFF41, 0x80);
    memory->write_byte(0xFF40, 0x91);

    memory->timer.div = 0xD3;
    memory->timer.tima = 0x00;
    memory->timer.tma = 0x00;
    memory->timer.tac = 0xF8;
}

void CPU::step(){
    if(memory->is_halted){
        memory->clock.t_instr = 4;
        return;
    }

    uint8_t instruction = this->memory->read_byte(registers->pc++);

    instructions->execute(instruction);
}     

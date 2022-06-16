#include <cstdint>
#include <iostream>

#include "Registers.h"
#include "instructions.h"

using namespace std;

InstructionSet::InstructionSet(Registers *registers, Interrupts* interrupts,  MMU *mmu, bool *stopped){
    this->registers = registers;
    this->mmu = mmu;
    this->interrupts = interrupts;
    this->stopped = stopped;
}
void InstructionSet::execute(uint8_t opcode) {
    mmu->clock.t += instructionTicks[opcode];
    // this->registers->m = instructionTicks[opcode] / 4;
    switch (opcode) {
        case 0x00: // NOP
            break;
        case 0x01: // LD BC, nn
            registers->bc = mmu->read_short(registers->pc);
            registers->pc += 2;
            break;
        case 0x02: // LD (BC), A
            mmu->write_byte(registers->bc, registers->a);
            break;
        case 0x03: // INC BC
            registers->bc++;
            break;
        case 0x04: // INC B
            inc(&registers->b); 
            break;
        case 0x05: // DEC B
            dec(&registers->b); 
            break;
        case 0x06: // LD B, n
            registers->b = mmu->read_byte(registers->pc++); 
            break;
        case 0x07: // RLCA
            rlc(&registers->a);
            registers->unset_register_flags(FLAG_ZERO);
            break;
        case 0x08: // LD (nn), SP
            mmu->write_short(mmu->read_short(registers->pc), registers->sp);
            registers->pc += 2;
            break;
        case 0x09: // ADD HL, BC
            add(&registers->hl, registers->bc);
            break;
        case 0x0A: // LD A, (BC)
            registers->a = mmu->read_byte(registers->bc);
            break;
        case 0x0B: // DEC BC
            registers->bc--;
            break;
        case 0x0C: // INC C
            inc(&registers->c); 
            break;
        case 0x0D: // DEC C
            dec(&registers->c);
            break;
        case 0x0E: // LD C, n
            registers->c = mmu->read_byte(registers->pc++); 
            break;
        case 0x0F: // RRCA
            rrc(&registers->a);
            registers->unset_register_flags(FLAG_ZERO);
            break;
        case 0x10: // STOP
            // TODO: Implement STOP
            break;
        case 0x11: // LD DE, nn
            registers->de = mmu->read_short(registers->pc);
            registers->pc += 2;
            break;
        case 0x12: // LD (DE), A
            mmu->write_byte(registers->de, registers->a);
            break;
        case 0x13: // INC DE
            registers->de++;
            break;
        case 0x14: // INC D
            inc(&registers->d);
            break;
        case 0x15: // DEC D
            dec(&registers->d);
            break;
        case 0x16: // LD D, n
            registers->d = mmu->read_byte(registers->pc++); 
            break;
        case 0x17: // RLA
            rl(&registers->a);
            registers->unset_register_flags(FLAG_ZERO);
            break;
        // case 0x18: // JR n
        //     registers->pc += (signed char)(mmu->read_byte(registers->pc++));
        //     break;

        case 0x18: // JR nn
        {
            uint8_t operand = mmu->read_byte(registers->pc++);
            registers->pc += (signed char)(operand);
        }
        break;
        case 0x19: // ADD HL, DE
            add(&registers->hl, registers->de);
            break;
        case 0x1A: // LD A, (DE)
            registers->a = mmu->read_byte(registers->de);
            break;
        case 0x1B: // DEC DE
            registers->de--;
            break;
        case 0x1C: // INC E
            inc(&registers->e); 
            break;
        case 0x1D: // DEC E
            dec(&registers->e);
            break;
        case 0x1E: // LD E, n
            registers->e = mmu->read_byte(registers->pc++); 
            break;
        case 0x1F: // RRA
            rr(&registers->a);
            registers->unset_register_flags(FLAG_ZERO);
            break;
        case 0x20: // JR NZ, *
            jump_add(!registers->is_set_register_flag(FLAG_ZERO));
            break;
        case 0x21: // LD HL, nn
            registers->hl = mmu->read_short(registers->pc);
            registers->pc += 2;
            break;
        case 0x22: // LD (HLI), A | LD (HL+), A | LDI (HL), A
            mmu->write_byte(registers->hl++, registers->a);
            break;
        case 0x23: // INC HL
            registers->hl++; 
            break;
        case 0x24: // INC H
            inc(&registers->h); 
            break;
        case 0x25: // DEC H
            dec(&registers->h);
            break;
        case 0x26: // LD H, n
            registers->h = mmu->read_byte(registers->pc++);
            break;
        // // TODO: NOT WORKING
        case 0x27: // DAA
            {
                uint16_t value = registers->a;

                if(registers->is_set_register_flag(FLAG_SUBTRACT)){
                    if(registers->is_set_register_flag(FLAG_CARRY)) {
                        value -= 0x60;
                    }

                    if(registers->is_set_register_flag(FLAG_HALF_CARRY)) {
                        value -= 0x6;
                    }
                }else{
                    if(registers->is_set_register_flag(FLAG_CARRY) || value > 0x99) {
                        value += 0x60;
                        registers->set_register_flags(FLAG_CARRY);
                    }

                    if(registers->is_set_register_flag(FLAG_HALF_CARRY) || (value & 0xF) > 0x9) {
                        value += 0x6;
                    }

                }
                registers->a = value;

                if(registers->a) registers->unset_register_flags(FLAG_ZERO); 
                else registers->set_register_flags(FLAG_ZERO);

                registers->unset_register_flags(FLAG_HALF_CARRY);
                break;
            }
        case 0x28: // JR Z, *
            jump_add(registers->is_set_register_flag(FLAG_ZERO));
            break;
        case 0x29: // ADD HL, HL
            add(&registers->hl, registers->hl);
            break;
        case 0x2A: // LD A, (HL+)
            registers->a = mmu->read_byte(registers->hl++);
            break;
        case 0x2B: // DEC HL
            registers->hl--;
            break;
        case 0x2C: // INC L
            inc(&registers->l); 
            break;
        case 0x2D: // DEC L
            dec(&registers->l); 
            break;
        case 0x2E: // LD L, n
            registers->l = mmu->read_byte(registers->pc++); 
            break;
        case 0x2F: // CPL
            registers->a = ~registers->a; 
            registers->set_register_flags(FLAG_SUBTRACT | FLAG_HALF_CARRY);
            break;
        case 0x30: // JR NC, *
            jump_add(!registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0x31: // LD SP, nn
            registers->sp = mmu->read_short(registers->pc);
            registers->pc += 2;
            break;
        case 0x32: // LD (HLD), A | LD (HL-), A | LDD (HL), A
            mmu->write_byte(registers->hl--, registers->a);
            // registers->hl--;
            break;
        case 0x33: // INC SP
            registers->sp++;
            break;
        case 0x35: // DEC (HL)
            {
                uint8_t tmp_val = mmu->read_byte(registers->hl);
                dec(&tmp_val);
                mmu->write_byte(registers->hl, tmp_val);
                break;
            }
        case 0x34: // INC (HL)
            {
                uint8_t tmp_val = mmu->read_byte(registers->hl);
                inc(&tmp_val);
                mmu->write_byte(registers->hl, tmp_val); 
                break;
            }
        case 0x36: // LD (HL), n
            mmu->write_byte(registers->hl, mmu->read_byte(registers->pc++));
            break;
        case 0x37: // SCF
            registers->set_register_flags(FLAG_CARRY);
            registers->unset_register_flags(FLAG_SUBTRACT | FLAG_HALF_CARRY);
            break;
        case 0x38: // JR C, *
            jump_add(registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0x39: // ADD HL, SP
            add(&registers->hl, registers->sp);
            break;
        case 0x3A: // LD A, (HL-)
            registers->a = mmu->read_byte(registers->hl--);
            break;
        case 0x3B: // DEC SP
            registers->sp--;
            break;
        case 0x3C: // INC A
            inc(&registers->a); 
            break;
        case 0x3D: // DEC A
            dec(&registers->a);
            break;
        case 0x3E: // LD A, n
            registers->a = mmu->read_byte(registers->pc++);
            break;
        case 0x3F: // CCF
            if(registers->is_set_register_flag(FLAG_CARRY)) registers->unset_register_flags(FLAG_CARRY);
            else registers->set_register_flags(FLAG_CARRY);

            registers->unset_register_flags(FLAG_SUBTRACT | FLAG_HALF_CARRY);
            break;
        case 0x40: // LD B, B
            break;
        case 0x41: // LD B, C
            registers->b = registers->c;
            break;
        case 0x42: // LD B, D
            registers->b = registers->d;
            break;
        case 0x43: // LD B, E
            registers->b = registers->e;
            break;
        case 0x44: // LD B, H
            registers->b = registers->h;
            break;
        case 0x45: // LD B, L
            registers->b = registers->l;
            break;
        case 0x46: // LD B, (HL)
            registers->b = mmu->read_byte(registers->hl);
            break;
        case 0x47: // LD B, A
            registers->b = registers->a;
            break;
        case 0x48: // LD C, B
            registers->c = registers->b;
            break;
        case 0x49: // LD C, C
            break;
        case 0x4A: // LD C, D
            registers->c = registers->d;
            break;
        case 0x4B: // LD C, E
            registers->c = registers->e;
            break;
        case 0x4C: // LD C, H
            registers->c = registers->h;
            break;
        case 0x4D: // LD C, L
            registers->c = registers->l;
            break;
        case 0x4E: // LD C, (HL)
            registers->c = mmu->read_byte(registers->hl);
            break;
        case 0x4F: // LD C, A
            registers->c = registers->a;
            break;
        case 0x50: // LD D, B
            registers->d = registers->b;
            break;
        case 0x51: // LD D, C
            registers->d = registers->c;
            break;
        case 0x52: // LD D, D
            break;
        case 0x53: // LD D, E
            registers->d = registers->e;
            break;
        case 0x54: // LD D, H
            registers->d = registers->h;
            break;
        case 0x55: // LD D, L
            registers->d = registers->l;
            break;
        case 0x56: // LD D, (HL)
            registers->d = mmu->read_byte(registers->hl);
            break;
        case 0x57: // LD D, A
            registers->d = registers->a;
            break;
        case 0x58: // LD E, B
            registers->e = registers->b;
            break;
        case 0x59: // LD E, C
            registers->e = registers->c;
            break;
        case 0x5A: // LD E, D
            registers->e = registers->d;
            break;
        case 0x5B: // LD E, E
            break;
        case 0x5C: // LD E, H
            registers->e = registers->h;
            break;
        case 0x5D: // LD E, L
            registers->e = registers->l;
            break;
        case 0x5E: // LD E, (HL)
            registers->e = mmu->read_byte(registers->hl);
            break;
        case 0x5F: // LD E, A
            registers->e = registers->a;
            break;
        case 0x60: // LD H, B
            registers->h = registers->b;
            break;
        case 0x61: // LD H, C
            registers->h = registers->c;
            break;
        case 0x62: // LD H, D
            registers->h = registers->d;
            break;
        case 0x63: // LD H, E
            registers->h = registers->e;
            break;
        case 0x64: // LD H, H
            break;
        case 0x65: // LD H, L
            registers->h = registers->l;
            break;
        case 0x66: // LD H, (HL)
            registers->h = mmu->read_byte(registers->hl);
            break;
        case 0x67: // LD H, A
            registers->h = registers->a;
            break;
        case 0x68: // LD L, B
            registers->l = registers->b;
            break;
        case 0x69: // LD L, C
            registers->l = registers->c;
            break;
        case 0x6A: // LD L, D
            registers->l = registers->d;
            break;
        case 0x6B: // LD L, E
            registers->l = registers->e;
            break;
        case 0x6C: // LD L, H
            registers->l = registers->h;
            break;
        case 0x6D: // LD L, L
            break;
        case 0x6E: // LD L, (HL)
            registers->l = mmu->read_byte(registers->hl);
            break;
        case 0x6F: // LD L, A
            registers->l = registers->a;
            break;
        case 0x70: // LD (HL), B
            mmu->write_byte(registers->hl, registers->b);
            break;
        case 0x71: // LD (HL), C
            mmu->write_byte(registers->hl, registers->c);
            break;
        case 0x72: // LD (HL), D
            mmu->write_byte(registers->hl, registers->d);
            break;
        case 0x73: // LD (HL), E
            mmu->write_byte(registers->hl, registers->e);
            break;
        case 0x74: // LD (HL), H
            mmu->write_byte(registers->hl, registers->h);
            break;
        case 0x75: // LD (HL), L
            mmu->write_byte(registers->hl, registers->l);
            break;
        case 0x76: // HALT
            mmu->is_halted = true;
            break;
        case 0x77: // LD (HL), A
            mmu->write_byte(registers->hl, registers->a);
            break;
        case 0x78: // LD A, B
            registers->a = registers->b;
            break;
        case 0x79: // LD A, C
            registers->a = registers->c;
            break;
        case 0x7A: // LD A, D
            registers->a = registers->d;
            break;
        case 0x7B: // LD A, E
            registers->a = registers->e;
            break;
        case 0x7C: // LD A, H
            registers->a = registers->h;
            break;
        case 0x7D: // LD A, L
            registers->a = registers->l;
            break;
        case 0x7E: // LD A, (HL)
            registers->a = mmu->read_byte(registers->hl);
            break;
        case 0x7F: // LD A, A
            break;
        case 0x80: // ADD A, B
            add(&registers->a, registers->b); 
            break;
        case 0x81: // ADD A, C
            add(&registers->a, registers->c); 
            break;
        case 0x82: // ADD A, D
            add(&registers->a, registers->d); 
            break;
        case 0x83: // ADD A, E
            add(&registers->a, registers->e); 
            break;
        case 0x84: // ADD A, H
            add(&registers->a, registers->h); 
            break;
        case 0x85: // ADD A, L
            add(&registers->a, registers->l); 
            break;
        case 0x86: // ADD A, (HL)
            add(&registers->a, mmu->read_byte(registers->hl)); 
            break;
        case 0x87: // ADD A, A
            add(&registers->a, registers->a); 
            break;
        case 0x88: // ADC A, B
            adc(registers->b); 
            break;
        case 0x89: // ADC A, C
            adc(registers->c); 
            break;
        case 0x8A: // ADC A, D
            adc(registers->d); 
            break;
        case 0x8B: // ADC A, E
            adc(registers->e); 
            break;
        case 0x8C: // ADC A, H
            adc(registers->h); 
            break;
        case 0x8D: // ADC A, L
            adc(registers->l); 
            break;
        case 0x8E: // ADC A, (HL)
            adc(mmu->read_byte(registers->hl)); 
            break;
        case 0x8F: // ADC A, A
            adc(registers->a); 
            break;
        case 0x90: // SUB B
            sub(registers->b); 
            break;
        case 0x91: // SUB C
            sub(registers->c); 
            break;
        case 0x92: // SUB D
            sub(registers->d); 
            break;
        case 0x93: // SUB E
            sub(registers->e); 
            break;
        case 0x94: // SUB H
            sub(registers->h); 
            break;
        case 0x95: // SUB L
            sub(registers->l); 
            break;
        case 0x96: // SUB (HL)
            sub(mmu->read_byte(registers->hl)); 
            break;
        case 0x97: // SUB A
            sub(registers->a); 
            break;
        case 0x98: // SBC A, B
            sbc(registers->b); 
            break;
        case 0x99: // SBC A, C
            sbc(registers->c); 
            break;
        case 0x9A: // SBC A, D
            sbc(registers->d); 
            break;
        case 0x9B: // SBC A, E
            sbc(registers->e); 
            break;
        case 0x9C: // SBC A, H
            sbc(registers->h); 
            break;
        case 0x9D: // SBC A, L
            sbc(registers->l); 
            break;
        case 0x9E: // SBC A, (HL)
            sbc(mmu->read_byte(registers->hl)); 
            break;
        case 0x9F: // SBC A, A
            sbc(registers->a); 
            break;
        case 0xA0: // AND B
            and_(registers->b); 
            break;
        case 0xA1: // AND C
            and_(registers->c); 
            break;
        case 0xA2: // AND D
            and_(registers->d); 
            break;
        case 0xA3: // AND E
            and_(registers->e); 
            break;
        case 0xA4: // AND H
            and_(registers->h); 
            break;
        case 0xA5: // AND l
            and_(registers->l); 
            break;
        case 0xA6: // AND (HL)
            and_(mmu->read_byte(registers->hl)); 
            break;
        case 0xA7: // AND A
            and_(registers->a); 
            break;
        case 0xA8: // XOR B
            xor_(registers->b);
            break;
        case 0xA9: // XOR C
            xor_(registers->c);
            break;
        case 0xAA: // XOR D
            xor_(registers->d);
            break;
        case 0xAB: // XOR E
            xor_(registers->e);
            break;
        case 0xAC: // XOR H
            xor_(registers->h);
            break;
        case 0xAD: // XOR L
            xor_(registers->l);
            break;
        case 0xAE: // XOR (HL)
            xor_(mmu->read_byte(registers->hl));
            break;
        case 0xAF: // XOR A
            xor_(registers->a);
            break;
        case 0xB0: // OR B
            or_(registers->b); 
            break;
        case 0xB1: // OR C
            or_(registers->c); 
            break;
        case 0xB2: // OR D
            or_(registers->d); 
            break;
        case 0xB3: // OR E
            or_(registers->e); 
            break;
        case 0xB4: // OR H
            or_(registers->h); 
            break;
        case 0xB5: // OR L
            or_(registers->l); 
            break;
        case 0xB6: // OR (HL)
            or_(mmu->read_byte(registers->hl));
            break;
        case 0xB7: // OR A
            or_(registers->a); 
            break;
        case 0xB8: // CP B
            cp(registers->b);
            break;
        case 0xB9: // CP C
            cp(registers->c);
            break;
        case 0xBA: // CP D
             cp(registers->d);
             break;
        case 0xBB: // CP E
            cp(registers->e);
            break;
        case 0xBC: // CP H
            cp(registers->h);
            break;
        case 0xBD: // CP L
            cp(registers->l);
            break;
        case 0xBE: // CP (HL)
            cp(mmu->read_byte(registers->hl)); 
            break;
        case 0xBF: // CP A
            cp(registers->a);
            break;
        case 0xC0: // RET NZ
            ret(!registers->is_set_register_flag(FLAG_ZERO));
            break;
        case 0xC1: // POP BC
            registers->bc = mmu->read_short_stack(&registers->sp);
            break;
        case 0xC2: // JP NZ, nn
            jump(!registers->is_set_register_flag(FLAG_ZERO));
            break ;
        case 0xC3: // JP nn
            // std::cout << "JUMPING TO " << +mmu->read_short(registers->pc) << "\n";
            registers->pc = mmu->read_short(registers->pc);
            break;
        case 0xC4: // CALL NZ, nn
            call(!registers->is_set_register_flag(FLAG_ZERO));
            break;
        case 0xC5: // PUSH BC
            mmu->write_short_stack(&registers->sp, registers->bc);
            break;
        case 0xC6: // ADD A, n 
            add(&registers->a, mmu->read_byte(registers->pc++)); 
            break;
        case 0xC7: // RST $00
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0000;
            break;
        case 0xC8: // RET Z
            ret(registers->is_set_register_flag(FLAG_ZERO));
            break;
        case 0xC9: // RET
            registers->pc = mmu->read_short_stack(&registers->sp);
            break;
        case 0xCA: // JP Z, nn
            jump(registers->is_set_register_flag(FLAG_ZERO));
            break;
        case 0xCB:
            extended_execute(mmu->read_byte(registers->pc++));
            // registers->pc++;
            break;
        case 0xCC: // CALL Z, nn
            call(registers->is_set_register_flag(FLAG_ZERO));
            break;
        // case 0xCD: // CALL nn
        //     // write_short_stack(registers->pc);
        //     // registers->pc = memory->read_short(registers->pc);
        //     // {
        //     // uint16_t operand = mmu->read_short(registers->pc);
        //     // registers->pc +=2;
        //     // mmu->write_short_stack(&registers->sp, registers->pc + 2);
        //     mmu->write_short_stack(&registers->sp, registers->pc + 2);
        //     registers->pc = mmu->read_short(registers->pc);
        //     // }
        //     break;

        case 0xCD: // CALL nn
        // write_short_stack(registers->pc);
        // registers->pc = memory->read_short(registers->pc);
        {
            uint16_t operand = mmu->read_short(registers->pc);
            registers->pc +=2;
            mmu->write_short_stack(&registers->sp, registers->pc);
            registers->pc = operand;
        }
        break;
        case 0xCE: // ADC A, n
            adc(mmu->read_byte(registers->pc++)); 
            break;
        case 0xCF: // RST $08
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0008;
            break;
        case 0xD0: // RET NC
            ret(!registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0xD1: // POP DE
            registers->de = mmu->read_short_stack(&registers->sp);
            break;
        case 0xD2: // JP NC, nn
            jump(!registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0xD4: // CALL NC, nn
            call(!registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0xD5: // PUSH DE
            mmu->write_short_stack(&registers->sp, registers->de);
            break;
        case 0xD6: // SUB n
            sub(mmu->read_byte(registers->pc++)); 
            break;
        case 0xD7: // RST $10
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0010;
            break;
        case 0xD8: // RET C
            ret(registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0xD9: // RETI
            interrupts->set_master_flag(true);
            registers->pc = mmu->read_short_stack(&registers->sp);
            break;
        case 0xDA: // JP C, nn
            jump(registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0xDC: // CALL C, nn
            call(registers->is_set_register_flag(FLAG_CARRY));
            break;
        case 0xDE: // SUB n
            sbc(mmu->read_byte(registers->pc++)); 
            break;
        case 0xDF: // RST $18
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0018;
            break;
        case 0xE0: // LD ($FF00+n), A
            mmu->write_byte(0xff00 + mmu->read_byte(registers->pc++), registers->a);
            break;
        case 0xE1: // POP HL
            registers->hl = mmu->read_short_stack(&registers->sp);
            break;
        case 0xE2: // LD ($FF00+C), A
            mmu->write_byte(0xff00 + registers->c, registers->a);
            break;
        case 0xE5: // PUSH HL
            mmu->write_short_stack(&registers->sp, registers->hl);
            break;
        case 0xE6: // AND n
            and_(mmu->read_byte(registers->pc++)); 
            break;
        case 0xE7: // RST $20
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0020;
           break;
        case 0xE8: // ADD SP, n
            add(&registers->sp,(signed char) mmu->read_byte(registers->pc++)); 
            break;
        case 0xE9: // JP HL
            registers->pc = registers->hl;
            break;
        case 0xEA: // LD (nn), A
            mmu->write_byte(mmu->read_short(registers->pc), registers->a);
            registers->pc += 2;
            break;
        case 0xEE: // XOR n
            xor_(mmu->read_byte(registers->pc++));
            break;
        case 0xEF: // RST $28
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0028;
            break;
        case 0xF0: // LD A, ($FF00+n)
            registers->a = mmu->read_byte(0xff00 + mmu->read_byte(registers->pc++));
            break;
        case 0xF1: // POP AF
            registers->af = mmu->read_short_stack(&registers->sp);
            registers->f &= 0xf0; // Reset the 4 unused bits
            break;
        case 0xF2: // LD A, (C)
            registers->a = mmu->read_byte(0xff00 + registers->c);
            break;
        case 0xF3: // DI
            interrupts->set_master_flag(false);
            break;
        case 0xF5: // PUSH AF
            mmu->write_short_stack(&registers->sp, registers->af);
            break;
        case 0xF6: // OR n
            or_(mmu->read_byte(registers->pc++)); 
            break;
        case 0xF7: // RST $30
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0030;
           break;
        case 0xF8: // LDHL SP, n
            ldhl(mmu->read_byte(registers->pc++));
            break;
        case 0xF9: // LD SP, HL
            registers->sp = registers->hl;
            break;
        case 0xFA: // LD A, (nn)
            registers->a = mmu->read_byte(mmu->read_short(registers->pc));
            registers->pc += 2;
            break;
        case 0xFB: // NI
            interrupts->set_master_flag(true);
            break;
        case 0xFE: // CP n
            cp_n(mmu->read_byte(registers->pc++));
            break;
        case 0xFF: // RST $38
            mmu->write_short_stack(&registers->sp, registers->pc++);
            registers->pc = 0x0038;
            break;
        // // case 0xFA: // LD A, n
        // //     registers->a = mmu->read_byte(registers->pc++);
        // //     break;
        case 0xFC: // ERROR: INVALID OPERATION
            break;
        // case 0xec: // ERROR: INVALID OPERATION
        //     break;
        default:
            std::cout << "FF40: " << +(mmu->read_byte(0xFF40)) << " FF41: " << +(mmu->read_byte(0xFF41)) << " FF42: " << +(mmu->read_byte(0xFF42)) << " FF44: " << +(mmu->read_byte(0xFF44)) << std::endl;
            registers->print_flags();
            registers->print_registers();
            // printf("Unsupported opcode: 0x%02x at 0x%04x\n\n\n", opcode, this->registers->pc);
            printf("Unsupported opcode: 0x%02x at 0x%04x\n", opcode, this->registers->pc);
            printf("DIV: %d\n", mmu->timer.div);
            printf("Cycles: %d \n\n\n", mmu->clock.t);
            std::exit(EXIT_FAILURE);
            return;
            break;
    }
}

void InstructionSet::write_short_stack(uint16_t value) {
    mmu->write_short_stack(&registers->sp, value);
}

uint16_t InstructionSet::read_short_stack() {
    return mmu->read_short_stack(&registers->sp);
    
}
// 8 bit arithmetics
// void InstructionSet::adc(uint8_t value) {
//     value += registers->is_set_register_flags(FLAG_CARRY) ? 1 : 0;
    
//     int result = registers->a + value;
    
//     if(result & 0xff00) registers->set_register_flags(FLAG_CARRY);
//     else registers->unset_register_flags(FLAG_CARRY);
    
//     if(value == registers->a) registers->set_register_flags(FLAG_ZERO);
//     else registers->unset_register_flags(FLAG_ZERO);
    
//     if(((value & 0x0f) + (registers->a & 0x0f)) > 0x0f) registers->set_register_flags(FLAG_HALF_CARRY);
//     else registers->unset_register_flags(FLAG_HALF_CARRY);
    
//     registers->set_register_flags(FLAG_SUBTRACT);
    
//     registers->a = (unsigned char)(result & 0xff);
// }



// void InstructionSet::add(uint16_t* destination, uint16_t value){
//     int result = *destination + value;
//     
//     // if(!registers->is_set_register_flags(FLAG_ZERO)){
//     //     registers->unset_register_flags(FLAG_CARRY);
//     //     registers->unset_register_flags(FLAG_ZERO);
//     //     registers->unset_register_flags(FLAG_HALF_CARRY);
//     //     registers->unset_register_flags(FLAG_SUBTRACT);
//     // }
//     registers->unset_register_flags(FLAG_CARRY);
//     // registers->unset_register_flags(FLAG_ZERO);
//     registers->unset_register_flags(FLAG_HALF_CARRY);
//     registers->unset_register_flags(FLAG_SUBTRACT);
//
//     if(result & 0x10000) registers->set_register_flags(FLAG_CARRY);
//
//
//     if((*destination ^ value ^ (result & 0xFFFF)) & 0x10000) registers->set_register_flags(FLAG_HALF_CARRY);
//
//
//     // if(*destination)registers->unset_register_flags(FLAG_ZERO);
//     // else registers->set_register_flags(FLAG_ZERO);
//     
//     // registers->unset_register_flags(FLAG_SUBTRACT);
//
//     *destination = (uint16_t)(result & 0xffff);
// }


/* void InstructionSet::add(uint16_t* destination, uint16_t value){
    int result = *destination + value;
    if(registers->is_set_register_flags(FLAG_ZERO)){
        registers->set_register_flags(FLAG_ZERO);
    }else{
        registers->unset_register_flags(FLAG_SUBTRACT);
        registers->unset_register_flags(FLAG_CARRY);
        registers->unset_register_flags(FLAG_HALF_CARRY);
        registers->unset_register_flags(FLAG_ZERO);
    }

    if(result & 0x10000) registers->set_register_flags(FLAG_CARRY);

    if ((*destination ^ value ^ (result & 0xFFFF)) & 0x1000) registers->set_register_flags(FLAG_HALF_CARRY);

    *destination = (unsigned short)(result & 0xffff);
} */





void InstructionSet::call(bool condition){
    uint16_t operand = mmu->read_short(registers->pc);
    registers->pc += 2;
    
    mmu->clock.t += 12;
    if(condition){
        mmu->write_short_stack(&registers->sp, registers->pc);
        registers->pc = operand;
        mmu->clock.t += 12;
    }
    // if(!registers->is_set_register_flag(FLAG_ZERO)){
    //     registers->pc = (signed short)(mmu->read_short(registers->pc));
    // }else{
    //     registers->pc+=2;
    // }
}


void InstructionSet::ret(bool condition){
    if(condition){
        registers->pc = mmu->read_short_stack(&registers->sp);
        mmu->clock.t += 20;
    }else{
        mmu->clock.t += 8;
    }
}

void InstructionSet::jump_add(bool condition){
    if(condition){
        registers->pc += 1 + (signed char)(mmu->read_byte(registers->pc));
        // registers->pc++;
        mmu->clock.t += 12;
    }else{
        registers->pc++;
        mmu->clock.t += 8;
    }
}

void InstructionSet::jump(bool condition){
    if(condition){
        registers->pc = mmu->read_short(registers->pc);
        mmu->clock.t += 16;
    }else{
        registers->pc+=2;
        mmu->clock.t += 12;
    }
}
void InstructionSet::inc(uint8_t *value){
    // *value += 1;

    // if((*value & 0x0F) == 0x00) registers->set_register_flags(FLAG_HALF_CARRY);
    // else registers->unset_register_flags(FLAG_HALF_CARRY);
    
    
    // if(*value) registers->unset_register_flags(FLAG_ZERO);
    // else registers->set_register_flags(FLAG_ZERO);

    // registers->unset_register_flags(FLAG_SUBTRACT);
    //
    // TODO: Change this
    if((*value & 0x0f) == 0x0f) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);
    
    *value += 1;
    
    if(*value) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->unset_register_flags(FLAG_SUBTRACT);
}

void InstructionSet::dec(uint8_t *value){
    if(*value & 0x0f) registers->unset_register_flags(FLAG_HALF_CARRY);
    else registers->set_register_flags(FLAG_HALF_CARRY);
    
    *value -= 1;

    if(*value) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->set_register_flags(FLAG_SUBTRACT);
}

void InstructionSet::cp(uint8_t value){
    uint8_t temp_val = registers->a;
    if(value > temp_val) registers->set_register_flags(FLAG_CARRY);
    else registers->unset_register_flags(FLAG_CARRY);
    
    if((value & 0x0f) > (temp_val & 0x0f)) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);
    
    temp_val -= value;
    
    if(temp_val) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->set_register_flags(FLAG_SUBTRACT);
}

void InstructionSet::add(uint8_t *destination, uint8_t value){
    uint16_t result = *destination + value;

    if(result > 0xff) registers->set_register_flags(FLAG_CARRY);
    else registers->unset_register_flags(FLAG_CARRY);

    if(((*destination & 0x0f) + (value & 0x0f)) > 0x0f) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);

    *destination = result;
    if(*destination) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->unset_register_flags(FLAG_SUBTRACT);
}

void InstructionSet::add(uint16_t *destination, uint16_t value){
    uint32_t result = *destination + value;
    
    if(result > 0xffff) registers->set_register_flags(FLAG_CARRY);
    else registers->unset_register_flags(FLAG_CARRY);
    
    if(((*destination & 0x0fff) + (value & 0x0fff)) > 0x0fff) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);

    *destination = (uint16_t)result;

    registers->unset_register_flags(FLAG_SUBTRACT);
}

// TODO: Change signed char to another type
void InstructionSet::add(uint16_t *destination, signed char value){
    uint16_t result = *destination + (signed char)value;

    registers->unset_register_flags(FLAG_SUBTRACT | FLAG_ZERO | FLAG_CARRY | FLAG_HALF_CARRY);

    if (((registers->sp ^ value ^ (result & 0xFFFF)) & 0x100) == 0x100)
        registers->set_register_flags(FLAG_CARRY);
    
    if (((registers->sp ^ value ^ (result & 0xFFFF)) & 0x10) == 0x10)
        registers->set_register_flags(FLAG_HALF_CARRY);

    *destination = result & 0xFFFF;
}
// TODO: Change signed char to another type
void InstructionSet::ldhl(signed char value){
    uint16_t result = registers->sp + value;

    registers->unset_register_flags(FLAG_SUBTRACT | FLAG_ZERO | FLAG_CARRY | FLAG_HALF_CARRY);

    if (((registers->sp ^ value ^ result) & 0x100) == 0x100)
        registers->set_register_flags(FLAG_CARRY);
    
    if (((registers->sp ^ value ^ result) & 0x10) == 0x10)
        registers->set_register_flags(FLAG_HALF_CARRY);

    registers->hl = result;
}



// void InstructionSet::adc(uint8_t value){
//     bool is_carry = registers->is_set_register_flag(FLAG_CARRY);
//     uint16_t result = registers->a + value + is_carry;

//     if(result > 0xff) registers->set_register_flags(FLAG_CARRY);
//     else registers->unset_register_flags(FLAG_CARRY);

//     if(((registers->a & 0x0f) + (value & 0x0f) + is_carry) > 0x0f) registers->set_register_flags(FLAG_HALF_CARRY);
//     else registers->unset_register_flags(FLAG_HALF_CARRY);

//     registers->a = (uint8_t) result;

//     if(registers->a) registers->unset_register_flags(FLAG_ZERO);
//     else registers->set_register_flags(FLAG_ZERO);

//     registers->unset_register_flags(FLAG_SUBTRACT);
// }


void InstructionSet::adc(uint8_t value) {
    int carry = registers->is_set_register_flag(FLAG_CARRY) ? 1 : 0;
    int result = registers->a + value + carry;
    
    if((unsigned char) result) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    if(result > 0xFF) registers->set_register_flags(FLAG_CARRY);
    else registers->unset_register_flags(FLAG_CARRY);

    if(((registers->a & 0x0F) + (value & 0x0f) + carry) > 0x0F) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);

    registers->unset_register_flags(FLAG_SUBTRACT);
    
    registers->a = (unsigned char)(result & 0xff);
}

void InstructionSet::and_(uint8_t value){
    registers->a = registers->a & value;
    if(registers->a) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->set_register_flags(FLAG_HALF_CARRY);
    registers->unset_register_flags(FLAG_SUBTRACT | FLAG_CARRY);
}

// void InstructionSet::or_(uint8_t value){
//     uint8_t temp_val = registers->a | value;
//     if(temp_val) registers->unset_register_flags(FLAG_ZERO);
//     else registers->set_register_flags(FLAG_ZERO);

//     registers->a = temp_val;

//     registers->unset_register_flags(FLAG_SUBTRACT | FLAG_HALF_CARRY | FLAG_CARRY);
// }
void InstructionSet::or_(uint8_t value) {
    registers->a |= value;
    
    if(registers->a) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);
    
    registers->unset_register_flags(FLAG_CARRY);
    registers->unset_register_flags(FLAG_SUBTRACT);
    registers->unset_register_flags(FLAG_HALF_CARRY);
}

void InstructionSet::sub(uint8_t value){
    if(value > registers->a) registers->set_register_flags(FLAG_CARRY);
    else registers->unset_register_flags(FLAG_CARRY);
    
    if((value & 0x0f) > (registers->a & 0x0f)) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);
    
    registers->a -= value;
    
    if(registers->a) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->set_register_flags(FLAG_SUBTRACT);
}

void InstructionSet::sbc(uint8_t value){
    bool is_carry = registers->is_set_register_flag(FLAG_CARRY);

    if((value + is_carry) > registers->a) registers->set_register_flags(FLAG_CARRY);
    else registers->unset_register_flags(FLAG_CARRY);
    
    if(((value & 0x0f) + is_carry) > (registers->a & 0x0f)) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);
    
    registers->a -= (value + is_carry);
    
    if(registers->a) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->set_register_flags(FLAG_SUBTRACT);
}
void InstructionSet::xor_(uint8_t value){
    registers->a ^= value;

    if(registers->a) registers->unset_register_flags(FLAG_ZERO);
    else registers->set_register_flags(FLAG_ZERO);

    registers->unset_register_flags(FLAG_CARRY | FLAG_SUBTRACT | FLAG_HALF_CARRY);
}

void InstructionSet::cp_n(uint8_t operand) {
    registers->set_register_flags(FLAG_SUBTRACT);
    
    if(registers->a == operand) registers->set_register_flags(FLAG_ZERO);
    else  registers->unset_register_flags(FLAG_ZERO);
    
    if(operand > registers->a) registers->set_register_flags(FLAG_CARRY);
    else registers->unset_register_flags(FLAG_CARRY);
    
    if((operand & 0x0f) > (registers->a & 0x0f)) registers->set_register_flags(FLAG_HALF_CARRY);
    else registers->unset_register_flags(FLAG_HALF_CARRY);
}

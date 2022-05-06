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
    this->registers->t = instructionTicks[opcode];
    // this->registers->m = instructionTicks[opcode] / 4;
    switch (opcode) {
        case 0x00:
            // cout << "NOP" << endl;
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
            registers->b = inc(registers->b); 
            break;
        case 0x05: // DEC B
            registers->b = dec(registers->b); 
            break;
        case 0x06: // LD B, n
            registers->b = mmu->read_byte(registers->pc++); 
            break;
        case 0x07: // RLCA
            {
                unsigned char carry = (registers->a & 0x80) >> 7;
                if(carry) registers->set_register_flag(FLAG_CARRY);
                else registers->unset_register_flag(FLAG_CARRY);;
                
                registers->a <<= 1;
                registers->a += carry;
                
                registers->unset_register_flag(FLAG_SUBTRACT);
                registers->unset_register_flag(FLAG_ZERO);
                registers->unset_register_flag(FLAG_HALF_CARRY);
            } 
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
            registers->c = inc(registers->c); 
            break;
        case 0x0D: // DEC C
            registers->c = dec(registers->c);
            break;
        case 0x0E: // LD C, n
            registers->c = mmu->read_byte(registers->pc++); 
            break;
        case 0x0F: // RRCA
            {
                unsigned char carry = registers->a & 0x01;

                if(carry) registers->set_register_flag(FLAG_CARRY);
                else registers->unset_register_flag(FLAG_CARRY);;
                
                registers->a >>= 1;
                if(carry) registers->a |= 0x80;
                
                registers->unset_register_flag(FLAG_SUBTRACT);
                registers->unset_register_flag(FLAG_ZERO);
                registers->unset_register_flag(FLAG_HALF_CARRY);
            } 
            break;
        case 0x10: // STOP BUG: Possible bug here
            // *this->stopped = true; // TODO: CHANGE THIS;
            registers->pc++;
            break;
        case 0x11: // LD DE, nn
            registers->de = mmu->read_short(registers->pc);
            registers->pc+=2;
            break;
        case 0x12: // LD (DE), A
            mmu->write_byte(registers->de, registers->a);
            break;
        case 0x13: // INC DE
            registers->de++;
            break;
        case 0x14: // INC D 
            registers->d = inc(registers->d);
            break;
        case 0x15: // DEC D
            registers->d = dec(registers->d);
            break;
        case 0x16: // LD D, n
            registers->d = mmu->read_byte(registers->pc++); 
            break;
        case 0x17: // RLA
            rla();
            break;
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
            registers->e = inc(registers->e); 
            break;
        case 0x1D: // DEC E
            registers->e = dec(registers->e);
            break;
        case 0x1E: // LD E, n
            registers->e = mmu->read_byte(registers->pc++);
            break;
        case 0x1F: // RRA
            {
                // int carry = (registers->is_set_register_flag(FLAG_CARRY) ? 1 : 0) << 7;
                // uint8_t value = registers->a >> 1;
                // if(registers->is_set_register_flag(FLAG_CARRY))
                //     value |= 0x80;
            
                // if(registers->a & 0x01) registers->set_register_flag(FLAG_CARRY);
                // else registers->unset_register_flag(FLAG_CARRY);
                
                // registers->a >>= 1;
                // registers->a = value;
                            
                // registers->unset_register_flag(FLAG_SUBTRACT);
                // registers->unset_register_flag(FLAG_ZERO);
                // registers->unset_register_flag(FLAG_HALF_CARRY);

                uint8_t carry = registers->is_set_register_flag(FLAG_CARRY) ? 0x80 : 0x00;
                uint8_t result = registers->a;

                if((registers->a & 0x01) != 0) registers->set_register_flag(FLAG_CARRY); 
                else registers->unset_register_flag(FLAG_CARRY);
                    
                result >>= 1;    
                result |= carry;    
                 
                registers->a = result;
                registers->unset_register_flag(FLAG_SUBTRACT);
                registers->unset_register_flag(FLAG_ZERO);
                registers->unset_register_flag(FLAG_HALF_CARRY);
                break;
            }
        case 0x20: // JR NZ, *
            // if(registers->pc == 0xfb){
            //     registers->pc = 0x100;
            // }
            if(registers->is_set_register_flag(FLAG_ZERO)){
                registers->pc++; // TODO
                this->registers->t += 8;
            }else{
                uint8_t operand = mmu->read_byte(registers->pc++);
                registers->pc += (signed char)(operand);
                // registers->pc += (signed char)mmu->read_byte(registers->pc++);
                this->registers->t += 12;
            }
            break;
        case 0x21: // LD HL, nn
            registers->hl = mmu->read_short(registers->pc);
            registers->pc+=2;
            break;
        case 0x22: // LD (HLI), A | LD (HL+), A | LDI (HL), A
            mmu->write_byte(registers->hl++, registers->a);
            break;
        case 0x23: // INC HL
            registers->hl++; 
            break;
        case 0x24: // INC H
            registers->h = inc(registers->h); 
            break;
        case 0x25: // DEC H
            registers->h = dec(registers->h);
            break;
        case 0x26: // LD H, n
            registers->h = mmu->read_byte(registers->pc++);
            break;
        case 0x27: // DAA BUG: This is incorrectly implemented
            /* {
                int s = registers->a;
            
                if(!registers->is_set_register_flag(FLAG_SUBTRACT)) {
                    if(registers->is_set_register_flag(FLAG_HALF_CARRY) || (s & 0xF) > 9) 
                        s += 0x06;
                    if(registers->is_set_register_flag(FLAG_CARRY) || s > 0x9F) 
                        s += 0x60;
                }
                else {
                    if(registers->is_set_register_flag(FLAG_HALF_CARRY)) 
                        s = (s - 6) & 0xFF;

                    if(registers->is_set_register_flag(FLAG_CARRY)) 
                        s -= 0x60;
                }
                registers->unset_register_flag(FLAG_HALF_CARRY);
                if((s & 0x100) == 0x100) 
                    registers->set_register_flag(FLAG_CARRY);
                
                s &= 0xFF;
                if(s) registers->unset_register_flag(FLAG_ZERO);
                else registers->set_register_flag(FLAG_ZERO);

                registers->a = s;
            } */

            {
                unsigned short s = registers->a;
                
                if(registers->is_set_register_flag(FLAG_SUBTRACT)) {
                    if(registers->is_set_register_flag(FLAG_HALF_CARRY)) s = (s - 0x06)&0xFF;
                    if(registers->is_set_register_flag(FLAG_CARRY)) s -= 0x60;
                }
                else {
                    if(registers->is_set_register_flag(FLAG_HALF_CARRY) || (s & 0xF) > 9) s += 0x06;
                    if(registers->is_set_register_flag(FLAG_CARRY) || s > 0x9F) s += 0x60;
                }
                
                registers->a = s;
                registers->unset_register_flag(FLAG_HALF_CARRY);

                if(registers->a) registers->unset_register_flag(FLAG_ZERO);
                else registers->set_register_flag(FLAG_ZERO);
                
                if(s >= 0x100) registers->set_register_flag(FLAG_CARRY);
            }
            break;
        case 0x28: // JR Z, nn
            if(registers->is_set_register_flag(FLAG_ZERO)){
                uint8_t operand = mmu->read_byte(registers->pc++);
                registers->pc += (signed char)(operand);
                this->registers->t += 12;
            }else{
                registers->pc++;
                this->registers->t += 8;
            }
            break;
        case 0x29: // ADD HL, HL
            add(&registers->hl, registers->hl);
            break;
        case 0x2A: // LD A, (HLI) | LD A, (HL+) | LDI A, (HL)
            registers->a = mmu->read_byte(registers->hl++);
            break;
        case 0x2B: // DEC HL
            registers->hl--;
            break;
        case 0x2C: // INC L
            registers->l = inc(registers->l); 
            break;
        case 0x2D: // DEC L
            registers->l = dec(registers->l);
            break;
        case 0x2E: // LD L, n
            registers->l = mmu->read_byte(registers->pc++);
            break;
        case 0x2F: // CPL
            registers->a = ~registers->a; 
            registers->set_register_flag(FLAG_SUBTRACT);
            registers->set_register_flag(FLAG_HALF_CARRY);
            break;
        case 0x30: // JR NC, *
            // if(registers->pc == 0xfb){
            //     registers->pc = 0x101;
            // }
            if(registers->is_set_register_flag(FLAG_CARRY)){
                registers->pc++; // TODO
                this->registers->t += 8;
            }else{
                uint8_t operand = mmu->read_byte(registers->pc++);
                registers->pc += (signed char)(operand);
                // registers->pc += (signed char)mmu->read_byte(registers->pc++);
                this->registers->t += 12;
            }
            break;
        case 0x31: // LD SP, nn
            registers->sp = mmu->read_short(registers->pc);
            registers->pc+=2;
            break;
        case 0x32: // LD (HLD), A | LD (HL-), A | LDD (HL), A
            mmu->write_byte(registers->hl, registers->a);
            registers->hl--;
            break;
        case 0x33: // INC SP
            registers->sp++;
            break;
        case 0x34: // INC (HL)
            mmu->write_byte(registers->hl, inc(mmu->read_byte(registers->hl)));
            break;
        case 0x35: // DEC (HL)
            mmu->write_byte(registers->hl, dec(mmu->read_byte(registers->hl)));
            break;
        case 0x36: // LD (HL), n
            mmu->write_byte(registers->hl, mmu->read_byte(registers->pc++));
            break;
        case 0x37: // SCF
            registers->set_register_flag(FLAG_CARRY);
            registers->unset_register_flag(FLAG_SUBTRACT);
            registers->unset_register_flag(FLAG_HALF_CARRY);
            break;
        case 0x38: // JP C, n
            if(registers->is_set_register_flag(FLAG_CARRY)){
                registers->pc += mmu->read_byte(registers->pc++);
                this->registers->t += 12;
            }
            else{
                registers->pc++;
                this->registers->t += 8;
            }
            break;
        case 0x39: // ADD HL, SP
            add(&registers->hl, registers->sp);
            break;
        case 0x3A: // LDD A, (HL)
            registers->a = mmu->read_byte(registers->hl--);
            break;
        case 0x3B: // DEC SP
            registers->sp--;
            break;
        case 0x3C: // INC A
            registers->a = inc(registers->a);
            break;
        case 0x3D: // DEC A
            registers->a = dec(registers->a);
            break;
        case 0x3E: // LD A, n
            registers->a = mmu->read_byte(registers->pc++);
            break;
        case 0x3F: // CCF
            if(registers->is_set_register_flag(FLAG_CARRY)) registers->unset_register_flag(FLAG_CARRY);
            else registers->set_register_flag(FLAG_CARRY);

            registers->unset_register_flag(FLAG_SUBTRACT);
            registers->unset_register_flag(FLAG_HALF_CARRY);
            break;
        case 0x40: // LD B, B
            // registers->b = registers->b; // TODO: REMOVE THIS. ITS NOP
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
            // registers->c = registers->c; // TODO: REMOVE THIS. ITS NOP
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
            // registers->d = registers->d; // TODO: NOP
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
            // registers->e = registers->e; // TODO: NOP
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
            // registers->h = registers->h; // TODO: NOP
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
            // registers->l = registers->l; // TODO: NOP
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
        case 0x76: // HALT TODO: Check this
            // if(!mmu->interruptFlags.IME){
            //     registers += 1;
            // }
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
            // registers->a = registers->a; // TODO: NOP
            break;
        case 0x80: // ADD A, B
            add(&registers->a, registers->b);
            break;
        case 0x81: // ADD A, C
            add(&registers->a, registers->c);
            break;
        case 0x82: // ADD A, D
            add(&registers->a, registers->d);
            break; // FIX 
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
        case 0x88: // ADC B
            adc(registers->b);
            break;
        case 0x89: // ADC C
            adc(registers->c);
            break;
        case 0x8A: // ADC D
            adc(registers->d);
            break;
        case 0x8B: // ADC E
            adc(registers->e);
            break;
        case 0x8C: // ADC H
            adc(registers->h);
            break;
        case 0x8D: // ADC L
            adc(registers->l);
            break;
        case 0x8E: // ADC (HL)
            adc(mmu->read_byte(registers->hl));
            break;
        case 0x8F: // ADC A
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
        case 0x98: // SBC B
            sbc(registers->b);
            break;
        case 0x99: // SBC C
            sbc(registers->c);
            break;
        case 0x9A: // SBC D
            sbc(registers->d);
            break;
        case 0x9B: // SBC E
            sbc(registers->e);
            break;
        case 0x9C: // SBC H
            sbc(registers->h);
            break;
        case 0x9D: // SBC L
            sbc(registers->l);
            break;
        case 0x9E: // SBC (HL)
            sbc(mmu->read_byte(registers->hl));
            break;
        case 0x9F: // SBC A
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
        case 0xA5: // AND L
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
            if(registers->is_set_register_flag(FLAG_ZERO)) this->registers->t +=8;
            else{
                registers->pc = read_short_stack() ;
                this->registers->t += 20;
            }
            break;
        case 0xC1: // POP BC
            registers->bc = read_short_stack();
            break;
        case 0xC2: // JP NZ, nn
            if(registers->is_set_register_flag(FLAG_ZERO)){
                registers->pc+=2;
                this->registers->t += 12;
            }
            else{
                registers->pc = mmu->read_short(registers->pc);
                this->registers->t += 16;
            }
            break;
        case 0xC3: // JP nn
            registers->pc = mmu->read_short(registers->pc);
            break;
        case 0xC4: // JP Z, nn
            if(registers->is_set_register_flag(FLAG_ZERO)){
                registers->pc += 2;
                this->registers->t += 12;
            }
            else{
                write_short_stack(registers->pc+2);
                registers->pc = mmu->read_short(registers->pc) ;
                this->registers->t += 24;
            }
            break;
        case 0xC5: // PUSH BC
            write_short_stack(registers->bc);
            break;
        case 0xC6: // ADD A, n
            add(&registers->a, mmu->read_byte(registers->pc++));
            break;
        case 0xC7: // RST 0x00
            write_short_stack(registers->pc);
            registers->pc = 0x0000;
            break;
        case 0xC8: // RET N
            if(registers->is_set_register_flag(FLAG_ZERO)){
                registers->pc = read_short_stack() ;
                this->registers->t += 20;
            }
            else{
                this->registers->t +=8;
            }
            break;
        case 0xC9: // RET
            registers->pc = read_short_stack();
            break;
        case 0xCA: // JP Z, nn
            if(registers->is_set_register_flag(FLAG_ZERO)){
                registers->pc = mmu->read_short(registers->pc) ;
                this->registers->t += 16;
            }
            else{
                registers->pc += 2;
                this->registers->t += 12;
            }
            break;
        case 0xCB:
            opcode = mmu->read_byte(registers->pc++);
            extended_execute(opcode);
            break;
        case 0xCC: // CALL Z, nn
            {
                uint16_t operand = mmu->read_short(registers->pc);
                registers->pc +=2;

                if(registers->is_set_register_flag(FLAG_ZERO)){
                    write_short_stack(registers->pc);
                    registers->pc = operand;
                    this->registers->t += 24;
                }
                else{
                    this->registers->t += 12;
                }
            }
            break;
        case 0xCD: // CALL nn
            // write_short_stack(registers->pc);
            // registers->pc = mmu->read_short(registers->pc);
            {
                uint16_t operand = mmu->read_short(registers->pc);
                registers->pc +=2;
                write_short_stack(registers->pc);
                registers->pc = operand;
            }
            break;
        case 0xCE: // ADC n
            adc(mmu->read_byte(registers->pc++));
            break;
        case 0xCF: // RST TODO: CHANGE NAME
            write_short_stack(registers->pc);
            registers->pc = 0x0008;
            break;
        case 0xD0: // RET NC
            if(registers->is_set_register_flag(FLAG_CARRY)) this->registers->t +=8;
            else{
                registers->pc = read_short_stack() ;
                this->registers->t += 20;
            }
            break;
        case 0xD1: // POP DE
            registers->de = read_short_stack();
            break;
        case 0xD2: // JP NC, nn
            if(registers->is_set_register_flag(FLAG_CARRY)){
                registers->pc+=2;
                this->registers->t += 12;
            }
            else{
                registers->pc = mmu->read_short(registers->pc);
                this->registers->t += 16;
            }
            break;
        case 0xD4: // CALL nn
            if(!registers->is_set_register_flag(FLAG_CARRY)){
                write_short_stack(registers->pc+2);
                registers->pc = mmu->read_short(registers->pc);
                this->registers->t += 24;
            }
            else{
                this->registers->t += 12;
                registers->pc += 2;
            }
            break;
        case 0xD5: // PUSH DE
            write_short_stack(registers->de);
            break;
        case 0xD6: // SUB n
            sub(mmu->read_byte(registers->pc++));
            break;
        case 0xD7: // RST 0x10
            write_short_stack(registers->pc);
            registers->pc = 0x0010;
            break;
        case 0xD8: // RET C
            if(registers->is_set_register_flag(FLAG_CARRY)){
                registers->pc = read_short_stack() ;
                this->registers->t += 20;
            }
            else{
                this->registers->t +=8;
            }
            break;
        case 0xD9: // RETI // TODO: CHECK THIS INTERRUPT
            // mmu->interruptFlags.IME = 1;
            interrupts->set_master_flag(true);
            registers->pc = read_short_stack();
            break;
        case 0xDA: // JP C, nn
            if(registers->is_set_register_flag(FLAG_CARRY)){
                registers->pc = mmu->read_short(registers->pc) ;
                this->registers->t += 16;
            }
            else{
                registers->pc += 2;
                this->registers->t += 12;
            }
            break;
        case 0xDC: // CALL C, nn
            {
                uint16_t operand = mmu->read_short(registers->pc);
                registers->pc +=2;

                if(registers->is_set_register_flag(FLAG_CARRY)){
                    write_short_stack(registers->pc);
                    registers->pc = operand;
                    this->registers->t += 24;
                }
                else{
                    this->registers->t += 12;
                }
            }
            break;
        case 0xDE: // SBC n 
            sbc(mmu->read_byte(registers->pc++));
            break;
        case 0xDF: // RST TODO: CHANGE NAME
            write_short_stack(registers->pc);
            registers->pc = 0x0018;
            break;
        case 0xE0: // LD ($FF00+n), A
            mmu->write_byte(0xff00 + mmu->read_byte(registers->pc++), registers->a);
            break;
        case 0xE1: // POP HL
            registers->hl = read_short_stack();
            break;
        case 0xE2: // LD ($FF00+C), A
            mmu->write_byte(0xff00 + registers->c, registers->a);
            break;
        case 0xE5: // PUSH HL
            write_short_stack(registers->hl);
            break;
        case 0xE6: // AND n
            and_n(mmu->read_byte(registers->pc++));
            break;
        case 0xE7: // RST 0x20
            write_short_stack(registers->pc);
            registers->pc = 0x0020;
            break;
        case 0xE8: // ADD SP, n
            {
                signed char operand = mmu->read_byte(registers->pc++);
                int result = registers->sp + operand;

                registers->unset_register_flag(FLAG_CARRY);
                registers->unset_register_flag(FLAG_HALF_CARRY);
                registers->unset_register_flag(FLAG_ZERO);
                registers->unset_register_flag(FLAG_SUBTRACT);
         
                if (((registers->sp ^ operand ^ (result & 0xFFFF)) & 0x100) == 0x100)
                    registers->set_register_flag(FLAG_CARRY);
                
                if (((registers->sp ^ operand ^ (result & 0xFFFF)) & 0x10) == 0x10)
                    registers->set_register_flag(FLAG_HALF_CARRY);
                
                registers->sp = result & 0xffff;
            }
            break;
        case 0xE9: // JP HL
            registers->pc = registers->hl;
            break;
        case 0xEA: // LD (nn), A
            mmu->write_byte(mmu->read_short(registers->pc), registers->a);
            registers->pc+=2;
            break;
        case 0xEE: // XOR n
            xor_(mmu->read_byte(registers->pc++));
            break;
        case 0xED: // UNDEFINED
            // xor_(mmu->read_byte(registers->pc++));
            break;
        case 0xEF: // RST TODO: CHANGE NAME
            write_short_stack(registers->pc);
            registers->pc = 0x0028;
            break;
        case 0xF0: // LD A, ($FF00 + n)
            // cout << hex << +mmu->read_byte(0xff00 + mmu->read_byte(registers->pc))<< endl;
            registers->a = mmu->read_byte(0xff00 + mmu->read_byte(registers->pc++));
            break;
        case 0xF1: // POP AF
            registers->af = read_short_stack();
            registers->f &= 0xf0; // Reset the 4 unused bits
            break;
        case 0xF2: // LC A, (0xFF00 + C)
            registers->a = mmu->read_byte(0xFF00 + registers->c);
            break;
        case 0xF3: // DI
            interrupts->set_master_flag(false);
            break;
        case 0xF5: // PUSH AF
            write_short_stack(registers->af);
            break;
        case 0xF6: // OR n
            or_(mmu->read_byte(registers->pc++));
            break;
        case 0xF7: // RST 0x30
            write_short_stack(registers->pc);
            registers->pc = 0x0030;
            break;
        case 0xF8: // LD HL, (SP+n) 
            {

                signed char operand = mmu->read_byte(registers->pc++);
                int result = registers->sp + operand;
                
                registers->unset_register_flag(FLAG_ZERO);
                registers->unset_register_flag(FLAG_CARRY);
                registers->unset_register_flag(FLAG_HALF_CARRY);
                registers->unset_register_flag(FLAG_SUBTRACT);
        
                if (((registers->sp ^ operand ^ result) & 0x100) == 0x100)
                    registers->set_register_flag(FLAG_CARRY);
                
                if (((registers->sp ^ operand ^ result) & 0x10) == 0x10)
                    registers->set_register_flag(FLAG_HALF_CARRY);
                
                registers->hl = (unsigned short)(result & 0xffff);
            }
            break;
        case 0xF9: // LD SP, HL
            registers->sp = registers->hl;
            break;
        case 0xFA: // LD A, (nn)
            registers->a = mmu->read_byte(mmu->read_short(registers->pc));
            registers->pc+=2;
            break;
        case 0xFB: // EI
            interrupts->set_master_flag(true);
            break;
        case 0xFE:
            cp_n(mmu->read_byte(registers->pc++));
            break;
        case 0xFF:
            write_short_stack(registers->pc);
            registers->pc = 0x0038;
            break;
        // case 0xFA: // LD A, n
        //     registers->a = mmu->read_byte(registers->pc++);
        //     break;
        default:
            cout << "NOT IMPLEMENTED: " << hex << +opcode <<endl;
            cout << "PC: " << hex << +registers->pc <<endl;
            exit(1);
            // End of tileset loading function
            // FILE *f = fopen("hram.bin", "wb");
            // fwrite(mmu->hram, 8, sizeof(mmu->hram), f);
            // fclose(f);
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
void InstructionSet::add(uint8_t* destination, uint8_t value){
    unsigned int result = *destination + value;


    if(result & 0xff00) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);

    if(((*destination & 0x0f) + (value & 0x0f)) > 0x0f) registers->set_register_flag(FLAG_HALF_CARRY);
    else registers->unset_register_flag(FLAG_HALF_CARRY);

    *destination = (uint8_t)(result & 0xff);

    if(*destination)registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
}

void InstructionSet::sub(uint8_t value){
    registers->set_register_flag(FLAG_SUBTRACT);

    if(value > registers->a) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    if((value & 0x0f) > (registers->a & 0x0f)) registers->set_register_flag(FLAG_HALF_CARRY);
    else registers->unset_register_flag(FLAG_HALF_CARRY);
    
    registers->a -= value;
    
    if(registers->a) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
}

// void InstructionSet::adc(uint8_t value) {
//     value += registers->is_set_register_flag(FLAG_CARRY) ? 1 : 0;
    
//     int result = registers->a + value;
    
//     if(result & 0xff00) registers->set_register_flag(FLAG_CARRY);
//     else registers->unset_register_flag(FLAG_CARRY);
    
//     if(value == registers->a) registers->set_register_flag(FLAG_ZERO);
//     else registers->unset_register_flag(FLAG_ZERO);
    
//     if(((value & 0x0f) + (registers->a & 0x0f)) > 0x0f) registers->set_register_flag(FLAG_HALF_CARRY);
//     else registers->unset_register_flag(FLAG_HALF_CARRY);
    
//     registers->set_register_flag(FLAG_SUBTRACT);
    
//     registers->a = (unsigned char)(result & 0xff);
// }
void InstructionSet::adc(uint8_t value) {
    int carry = registers->is_set_register_flag(FLAG_CARRY) ? 1 : 0;
    int result = registers->a + value + carry;
    
    if((unsigned char) result) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);

    if(result > 0xFF) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);

    if(((registers->a & 0x0F) + (value & 0x0f) + carry) > 0x0F) registers->set_register_flag(FLAG_HALF_CARRY);
    else registers->unset_register_flag(FLAG_HALF_CARRY);

    registers->unset_register_flag(FLAG_SUBTRACT);
    
    registers->a = (unsigned char)(result & 0xff);
}

void InstructionSet::sbc(uint8_t value) {
    int carry = registers->is_set_register_flag(FLAG_CARRY) ? 1 : 0;
    int result = registers->a - value - carry;
    
    registers->unset_register_flag(FLAG_CARRY);
    registers->unset_register_flag(FLAG_HALF_CARRY);
    registers->unset_register_flag(FLAG_ZERO);
    registers->unset_register_flag(FLAG_SUBTRACT);

    registers->set_register_flag(FLAG_SUBTRACT);

    if(((unsigned char) result) == 0) registers->set_register_flag(FLAG_ZERO);
    // else registers->unset_register_flag(FLAG_ZERO);


    if(result < 0) registers->set_register_flag(FLAG_CARRY);
    // else registers->unset_register_flag(FLAG_CARRY);

    if(((registers->a & 0x0f) - (value & 0x0f) - carry) < 0) registers->set_register_flag(FLAG_HALF_CARRY);
    // else registers->unset_register_flag(FLAG_HALF_CARRY);

    registers->a = (unsigned char)(result & 0xff);
}

void InstructionSet::or_(uint8_t value) {
    registers->a |= value;
    
    if(registers->a) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_CARRY);
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);
}

void InstructionSet::xor_(uint8_t value){
    registers->a ^= value;

    if(registers->a) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);

    registers->unset_register_flag(FLAG_CARRY);
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);
}


uint8_t InstructionSet::inc(uint8_t value){
    if((value & 0x0f) == 0x0f) registers->set_register_flag(FLAG_HALF_CARRY);
    else registers->unset_register_flag(FLAG_HALF_CARRY);
    
    value++;
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);

    registers->unset_register_flag(FLAG_SUBTRACT);

    return value;
}

uint8_t InstructionSet::dec(uint8_t value){
    if(value & 0x0f) registers->unset_register_flag(FLAG_HALF_CARRY);
    else registers->set_register_flag(FLAG_HALF_CARRY);
    
    value--;

    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);

    registers->set_register_flag(FLAG_SUBTRACT);

    return value;
}

void InstructionSet::rla(){
    int carry = registers->is_set_register_flag(FLAG_CARRY) ? 1 : 0;

    if(registers->a & 0x80) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    registers->a <<= 1;
    registers->a += carry;

    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_ZERO);
    registers->unset_register_flag(FLAG_HALF_CARRY);
}

// 0xfe
void InstructionSet::cp_n(uint8_t operand) {
    registers->set_register_flag(FLAG_SUBTRACT);
    
    if(registers->a == operand) registers->set_register_flag(FLAG_ZERO);
    else  registers->unset_register_flag(FLAG_ZERO);
    
    if(operand > registers->a) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    if((operand & 0x0f) > (registers->a & 0x0f)) registers->set_register_flag(FLAG_HALF_CARRY);
    else registers->unset_register_flag(FLAG_HALF_CARRY);
}

void InstructionSet::cp(unsigned char value) {
    if(registers->a == value) registers->set_register_flag(FLAG_ZERO);
    else registers->unset_register_flag(FLAG_ZERO);
    
    if(value > registers->a) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    if((value & 0x0f) > (registers->a & 0x0f)) registers->set_register_flag(FLAG_HALF_CARRY);
    else registers->unset_register_flag(FLAG_HALF_CARRY);

    
    registers->set_register_flag(FLAG_SUBTRACT);
}

void InstructionSet::and_(uint8_t value) {
    registers->a &= value;
    
    if(registers->a) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);

    registers->unset_register_flag(FLAG_CARRY);
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->set_register_flag(FLAG_HALF_CARRY);
}

void InstructionSet::and_n(uint8_t operand) {
    registers->a &= operand;
    
    registers->unset_register_flag(FLAG_CARRY);
    registers->unset_register_flag(FLAG_SUBTRACT);

    registers->set_register_flag(FLAG_HALF_CARRY);

    if(registers->a) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
}


// void InstructionSet::add(uint16_t* destination, uint16_t value){
//     int result = *destination + value;
//     
//     // if(!registers->is_set_register_flag(FLAG_ZERO)){
//     //     registers->unset_register_flag(FLAG_CARRY);
//     //     registers->unset_register_flag(FLAG_ZERO);
//     //     registers->unset_register_flag(FLAG_HALF_CARRY);
//     //     registers->unset_register_flag(FLAG_SUBTRACT);
//     // }
//     registers->unset_register_flag(FLAG_CARRY);
//     // registers->unset_register_flag(FLAG_ZERO);
//     registers->unset_register_flag(FLAG_HALF_CARRY);
//     registers->unset_register_flag(FLAG_SUBTRACT);
//
//     if(result & 0x10000) registers->set_register_flag(FLAG_CARRY);
//
//
//     if((*destination ^ value ^ (result & 0xFFFF)) & 0x10000) registers->set_register_flag(FLAG_HALF_CARRY);
//
//
//     // if(*destination)registers->unset_register_flag(FLAG_ZERO);
//     // else registers->set_register_flag(FLAG_ZERO);
//     
//     // registers->unset_register_flag(FLAG_SUBTRACT);
//
//     *destination = (uint16_t)(result & 0xffff);
// }


void InstructionSet::add(uint16_t* destination, uint16_t value){
    uint32_t result = *destination + value;


    if(result > 0xffff) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);


    if(((*destination & 0x0fff) + (value & 0x0fff)) > 0x0fff) registers->set_register_flag(FLAG_HALF_CARRY);
    else registers->unset_register_flag(FLAG_HALF_CARRY);

    registers->unset_register_flag(FLAG_SUBTRACT);

    *destination = (unsigned short)(result & 0xffff);
}

/* void InstructionSet::add(uint16_t* destination, uint16_t value){
    int result = *destination + value;
    if(registers->is_set_register_flag(FLAG_ZERO)){
        registers->set_register_flag(FLAG_ZERO);
    }else{
        registers->unset_register_flag(FLAG_SUBTRACT);
        registers->unset_register_flag(FLAG_CARRY);
        registers->unset_register_flag(FLAG_HALF_CARRY);
        registers->unset_register_flag(FLAG_ZERO);
    }

    if(result & 0x10000) registers->set_register_flag(FLAG_CARRY);

    if ((*destination ^ value ^ (result & 0xFFFF)) & 0x1000) registers->set_register_flag(FLAG_HALF_CARRY);

    *destination = (unsigned short)(result & 0xffff);
} */

#include <iostream>

#include "instructions.h"

using namespace std;

void InstructionSet::extended_execute(uint8_t opcode){
    this->registers->t += extendedInstructionTicks[opcode] * 2;

    switch (opcode) {
        case 0x00: // RLC B
            registers->b = rlc(registers->b);
            break;
        case 0x01: // RLC C
            registers->c = rlc(registers->c);
            break;
        case 0x02: // RLC D
            registers->d = rlc(registers->d);
            break;
        case 0x03: // RLC E
            registers->e = rlc(registers->e);
            break;
        case 0x04: // RLC H
            registers->h = rlc(registers->h);
            break;
        case 0x05: // RLC L
            registers->l = rlc(registers->l);
            break;
        case 0x06: // RLC (HL)
            memory->write_byte(registers->hl, rlc(memory->read_byte(registers->hl)));
            break;
        case 0x07: // RLC A
            registers->a = rlc(registers->a);
            break;
        case 0x08: // RRC B
            registers->b = rrc(registers->b);
            break;
        case 0x09: // RRC C
            registers->c = rrc(registers->c);
            break;
        case 0x0A: // RRC D
            registers->d = rrc(registers->d);
            break;
        case 0x0B: // RRC E
            registers->e = rrc(registers->e);
            break;
        case 0x0C: // RRC H
            registers->h = rrc(registers->h);
            break;
        case 0x0D: // RRC L
            registers->l = rrc(registers->l);
            break;
        case 0x0E: // RRC (HL)
            memory->write_byte(registers->hl, rrc(memory->read_byte(registers->hl)));
            break;
        case 0x0F: // RRC A
            registers->a = rrc(registers->a);
            break;
        case 0x10: // RL B
            registers->b = rl(registers->b);
            break;
        case 0x11: // RL C
            registers->c = rl(registers->c);
            break;
        case 0x12: // RL D
            registers->d = rl(registers->d);
            break;
        case 0x13: // RL E
            registers->e = rl(registers->e);
            break;
        case 0x14: // RL H
            registers->h = rl(registers->h);
            break;
        case 0x15: // RL L
            registers->l = rl(registers->l);
            break;
        case 0x16: // RL (HL)
            memory->write_byte(registers->hl, rl(memory->read_byte(registers->hl)));
            break;
        case 0x17: // RL A
            registers->a = rl(registers->a);
            break;
        case 0x18: // RR B
            registers->b = rr(registers->b);
            break;
        case 0x19: // RR C
            registers->c = rr(registers->c);
            break;
        case 0x1A: // RR D
            registers->d = rr(registers->d);
            break;
        case 0x1B: // RR E
            registers->e = rr(registers->e);
            break;
        case 0x1C: // RR H
            registers->h = rr(registers->h);
            break;
        case 0x1D: // RR L
            registers->l = rr(registers->l);
            break;
        case 0x1E: // RR (HL)
            memory->write_byte(registers->hl, rr(memory->read_byte(registers->hl)));
            break;
        case 0x1F: // RR A
            registers->a = rr(registers->a);
            break;
        case 0x20: // SLA B
            registers->b = sla(registers->b);
            break;
        case 0x21: // SLA C
            registers->c = sla(registers->c);
            break;
        case 0x22: // SLA D
            registers->d = sla(registers->d);
            break;
        case 0x23: // SLA E
            registers->e = sla(registers->e);
            break;
        case 0x24: // SLA H
            registers->h = sla(registers->h);
            break;
        case 0x25: // SLA L
            registers->l = sla(registers->l);
            break;
        case 0x26: // SLA (HL)
            memory->write_byte(registers->hl, sla(memory->read_byte(registers->hl)));
            break;
        case 0x27: // SLA A
            registers->a = sla(registers->a);
            break;
        case 0x28: // SRA B
            registers->b = sra(registers->b);
            break;
        case 0x29: // SRA C
            registers->c = sra(registers->c);
            break;
        case 0x2A: // SRA D
            registers->d = sra(registers->d);
            break;
        case 0x2B: // SRA E
            registers->e = sra(registers->e);
            break;
        case 0x2C: // SRA H
            registers->h = sra(registers->h);
            break;
        case 0x2D: // SRA L
            registers->l = sra(registers->l);
            break;
        case 0x2E: // SRA (HL)
            memory->write_byte(registers->hl, sra(memory->read_byte(registers->hl)));
            break;
        case 0x2F: // SRA A
            registers->a = sra(registers->a);
            break;
        case 0x30: // SWAP B
            registers->b = swap(registers->b);
            break;
        case 0x31: // SWAP C
            registers->c = swap(registers->c);
            break;
        case 0x32: // SWAP D
            registers->d = swap(registers->d);
            break;
        case 0x33: // SWAP E
            registers->e = swap(registers->e);
            break;
        case 0x34: // SWAP H
            registers->h = swap(registers->h);
            break;
        case 0x35: // SWAP L
            registers->l = swap(registers->l);
            break;
        case 0x36: // SWAP (HL)
            memory->write_byte(registers->hl, swap(memory->read_byte(registers->hl)));
            break;
        case 0x37: // SWAP A
            registers->a = swap(registers->a);
            break;
        case 0x38: // SRL B
            registers->b = srl(registers->b);
            break;
        case 0x39: // SRL C
            registers->c = srl(registers->c);
            break;
        case 0x3A: // SRL D
            registers->d = srl(registers->d);
            break;
        case 0x3B: // SRL E
            registers->e = srl(registers->e);
            break;
        case 0x3C: // SRL H
            registers->h = srl(registers->h);
            break;
        case 0x3D: // SRL L
            registers->l = srl(registers->l);
            break;
        case 0x3E: // SRL (HL)
            memory->write_byte(registers->hl, srl(memory->read_byte(registers->hl)));
            break;
        case 0x3F: // SRL A
            registers->a = srl(registers->a);
            break;
        // case 0x3F: // SRL A
        //     if(registers->a & 0x01) registers->set_register_flag(FLAG_CARRY);
        //     else registers->unset_register_flag(FLAG_CARRY);
        //
        //     registers->a >>= 1;
        //
        //     if(registers->a) registers->unset_register_flag(FLAG_ZERO);
        //     else registers->set_register_flag(FLAG_ZERO);
        //
        //     registers->unset_register_flag(FLAG_SUBTRACT);
        //     registers->unset_register_flag(FLAG_HALF_CARRY);
        //     break;
        case 0x40: // BIT 0, B
            bit(1 << 0, registers->b);
            break;
        case 0x41: // BIT 0, C
            bit(1 << 0, registers->c);
            break;
        case 0x42: // BIT 0, D
            bit(1 << 0, registers->d);
            break;
        case 0x43: // BIT 0, E
            bit(1 << 0, registers->e);
            break;
        case 0x44: // BIT 0, H
            bit(1 << 0, registers->h);
            break;
        case 0x45: // BIT 0, L
            bit(1 << 0, registers->l);
            break;
        case 0x46: // BIT 0, (HL)
            bit(1 << 0, memory->read_byte(registers->hl));
            break;
        case 0x47: // BIT 0, A
            bit(1 << 0, registers->a);
            break;
        case 0x48: // BIT 1, B
            bit(1 << 1, registers->b);
            break;
        case 0x49: // BIT 1, C
            bit(1 << 1, registers->c);
            break;
        case 0x4A: // BIT 1, D
            bit(1 << 1, registers->d);
            break;
        case 0x4B: // BIT 1, E
            bit(1 << 1, registers->e);
            break;
        case 0x4C: // BIT 1, H
            bit(1 << 1, registers->h);
            break;
        case 0x4D: // BIT 1, L
            bit(1 << 1, registers->l);
            break;
        case 0x4E: // BIT 1, (HL)
            bit(1 << 1, memory->read_byte(registers->hl));
            break;
        case 0x4F: // BIT 1, A
            bit(1 << 1, registers->a);
            break;
        case 0x50: // BIT 2, B
            bit(1 << 2, registers->b);
            break;
        case 0x51: // BIT 2, C
            bit(1 << 2, registers->c);
            break;
        case 0x52: // BIT 2, D
            bit(1 << 2, registers->d);
            break;
        case 0x53: // BIT 2, E
            bit(1 << 2, registers->e);
            break;
        case 0x54: // BIT 2, H
            bit(1 << 2, registers->h);
            break;
        case 0x55: // BIT 2, L
            bit(1 << 2, registers->l);
            break;
        case 0x56: // BIT 2, (HL)
            bit(1 << 2, memory->read_byte(registers->hl));
            break;
        case 0x57: // BIT 2, A
            bit(1 << 2, registers->a);
            break;
        case 0x58: // BIT 3, B
            bit(1 << 3, registers->b);
            break;
        case 0x59: // BIT 3, C
            bit(1 << 3, registers->c);
            break;
        case 0x5A: // BIT 3, D
            bit(1 << 3, registers->d);
            break;
        case 0x5B: // BIT 3, E
            bit(1 << 3, registers->e);
            break;
        case 0x5C: // BIT 3, H
            bit(1 << 3, registers->h);
            break;
        case 0x5D: // BIT 3, L
            bit(1 << 3, registers->l);
            break;
        case 0x5E: // BIT 3, (HL)
            bit(1 << 3, memory->read_byte(registers->hl));
            break;
        case 0x5F: // BIT 3, A
            bit(1 << 3, registers->a);
            break;
        case 0x60: // BIT 4, B
            bit(1 << 4, registers->b);
            break;
        case 0x61: // BIT 4, C
            bit(1 << 4, registers->c);
            break;
        case 0x62: // BIT 4, D
            bit(1 << 4, registers->d);
            break;
        case 0x63: // BIT 4, E
            bit(1 << 4, registers->e);
            break;
        case 0x64: // BIT 4, H
            bit(1 << 4, registers->h);
            break;
        case 0x65: // BIT 4, L
            bit(1 << 4, registers->l);
            break;
        case 0x66: // BIT 4, (HL)
            bit(1 << 4, memory->read_byte(registers->hl));
            break;
        case 0x67: // BIT 4, A
            bit(1 << 4, registers->a);
            break;
        case 0x68: // BIT 5, B
            bit(1 << 5, registers->b);
            break;
        case 0x69: // BIT 5, C
            bit(1 << 5, registers->c);
            break;
        case 0x6A: // BIT 5, D
            bit(1 << 5, registers->d);
            break;
        case 0x6B: // BIT 5, E
            bit(1 << 5, registers->e);
            break;
        case 0x6C: // BIT 5, H
            bit(1 << 5, registers->h);
            break;
        case 0x6D: // BIT 5, L
            bit(1 << 5, registers->l);
            break;
        case 0x6E: // BIT 5, (HL)
            bit(1 << 5, memory->read_byte(registers->hl));
            break;
        case 0x6F: // BIT 5, A
            bit(1 << 5, registers->a);
            break;
        case 0x70: // BIT 6, B
            bit(1 << 6, registers->b);
            break;
        case 0x71: // BIT 6, C
            bit(1 << 6, registers->c);
            break;
        case 0x72: // BIT 6, D
            bit(1 << 6, registers->d);
            break;
        case 0x73: // BIT 6, E
            bit(1 << 6, registers->e);
            break;
        case 0x74: // BIT 6, H
            bit(1 << 6, registers->h);
            break;
        case 0x75: // BIT 6, L
            bit(1 << 6, registers->l);
            break;
        case 0x76: // BIT 6, (HL)
            bit(1 << 6, memory->read_byte(registers->hl));
            break;
        case 0x77: // BIT 6, A
            bit(1 << 6, registers->a);
            break;
        case 0x78: // BIT 7, B
            bit(1 << 7, registers->b);
            break;
        case 0x79: // BIT 7, C
            bit(1 << 7, registers->c);
            break;
        case 0x7A: // BIT 7, D
            bit(1 << 7, registers->d);
            break;
        case 0x7B: // BIT 7, E
            bit(1 << 7, registers->e);
            break;
        case 0x7C: // BIT 6, H
            bit(1 << 7, registers->h);
            break;
        case 0x7D: // BIT 7, L
            bit(1 << 7, registers->l);
            break;
        case 0x7E: // BIT 7, (HL)
            bit(1 << 7, memory->read_byte(registers->hl));
            break;
        case 0x7F: // BIT 7, A
            bit(1 << 7, registers->a);
            break;
        case 0x80: // RES 0, B
            registers->b &= ~(1 << 0);
            break;
        case 0x81: // RES 0, C
            registers->c &= ~(1 << 0);
            break;
        case 0x82: // RES 0, D
            registers->d &= ~(1 << 0);
            break;
        case 0x83: // RES 0, E
            registers->e &= ~(1 << 0);
            break;
        case 0x84: // RES 0, H
            registers->h &= ~(1 << 0);
            break;
        case 0x85: // RES 0, L
            registers->l &= ~(1 << 0);
            break;
        case 0x86: // RES 0, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 0));
            break;
        case 0x87: // RES 0, A
            registers->a &= ~(1 << 0);
            break;

        case 0x88: // RES 1, B
            registers->b &= ~(1 << 1);
            break;
        case 0x89: // RES 1, C
            registers->c &= ~(1 << 1);
            break;
        case 0x8A: // RES 1, D
            registers->d &= ~(1 << 1);
            break;
        case 0x8B: // RES 1, E
            registers->e &= ~(1 << 1);
            break;
        case 0x8C: // RES 1, H
            registers->h &= ~(1 << 1);
            break;
        case 0x8D: // RES 0, L
            registers->l &= ~(1 << 1);
            break;
        case 0x8E: // RES 1, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 1));
            break;
        case 0x8F: // RES 0, A
            registers->a &= ~(1 << 1);
            break;

        case 0x90: // RES 2, B
            registers->b &= ~(1 << 2);
            break;
        case 0x91: // RES 2, C
            registers->c &= ~(1 << 2);
            break;
        case 0x92: // RES 2, D
            registers->d &= ~(1 << 2);
            break;
        case 0x93: // RES 2, E
            registers->e &= ~(1 << 2);
            break;
        case 0x94: // RES 2, H
            registers->h &= ~(1 << 2);
            break;
        case 0x95: // RES 2, L
            registers->l &= ~(1 << 2);
            break;
        case 0x96: // RES 2, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 2));
            break;
        case 0x97: // RES 2, A
            registers->a &= ~(1 << 2);
            break;


        case 0x98: // RES 3, B
            registers->b &= ~(1 << 3);
            break;
        case 0x99: // RES 3, C
            registers->c &= ~(1 << 3);
            break;
        case 0x9A: // RES 3, D
            registers->d &= ~(1 << 3);
            break;
        case 0x9B: // RES 3, E
            registers->e &= ~(1 << 3);
            break;
        case 0x9C: // RES 3, H
            registers->h &= ~(1 << 3);
            break;
        case 0x9D: // RES 3, L
            registers->l &= ~(1 << 3);
            break;
        case 0x9E: // RES 3, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 3));
            break;
        case 0x9F: // RES 3, A
            registers->a &= ~(1 << 3);
            break;


        case 0xA0: // RES 4, B
            registers->b &= ~(1 << 4);
            break;
        case 0xA1: // RES 4, C
            registers->c &= ~(1 << 4);
            break;
        case 0xA2: // RES 4, D
            registers->d &= ~(1 << 4);
            break;
        case 0xA3: // RES 4, E
            registers->e &= ~(1 << 4);
            break;
        case 0xA4: // RES 4, H
            registers->h &= ~(1 << 4);
            break;
        case 0xA5: // RES 4, L
            registers->l &= ~(1 << 4);
            break;
        case 0xA6: // RES 4, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 4));
            break;
        case 0xA7: // RES 4, A
            registers->a &= ~(1 << 4);
            break;

        case 0xA8: // RES 5, B
            registers->b &= ~(1 << 5);
            break;
        case 0xA9: // RES 5, C
            registers->c &= ~(1 << 5);
            break;
        case 0xAA: // RES 5, D
            registers->d &= ~(1 << 5);
            break;
        case 0xAB: // RES 5, E
            registers->e &= ~(1 << 5);
            break;
        case 0xAC: // RES 5, H
            registers->h &= ~(1 << 5);
            break;
        case 0xAD: // RES 5, L
            registers->l &= ~(1 << 5);
            break;
        case 0xAE: // RES 5, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 5));
            break;
        case 0xAF: // RES 5, A
            registers->a &= ~(1 << 5);
            break;


        case 0xB0: // RES 6, B
            registers->b &= ~(1 << 6);
            break;
        case 0xB1: // RES 6, C
            registers->c &= ~(1 << 6);
            break;
        case 0xB2: // RES 6, D
            registers->d &= ~(1 << 6);
            break;
        case 0xB3: // RES 6, E
            registers->e &= ~(1 << 6);
            break;
        case 0xB4: // RES 6, H
            registers->h &= ~(1 << 6);
            break;
        case 0xB5: // RES 6, L
            registers->l &= ~(1 << 6);
            break;
        case 0xB6: // RES 6, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 6));
            break;
        case 0xB7: // RES 6, A
            registers->a &= ~(1 << 6);
            break;

        case 0xB8: // RES 7, B
            registers->b &= ~(1 << 7);
            break;
        case 0xB9: // RES 7, C
            registers->c &= ~(1 << 7);
            break;
        case 0xBA: // RES 7, D
            registers->d &= ~(1 << 7);
            break;
        case 0xBB: // RES 7, E
            registers->e &= ~(1 << 7);
            break;
        case 0xBC: // RES 7, H
            registers->h &= ~(1 << 7);
            break;
        case 0xBD: // RES 7, L
            registers->l &= ~(1 << 7);
            break;
        case 0xBE: // RES 7, (HL)
            memory->write_byte(registers->hl, memory->read_byte(registers->hl) & ~(1 << 7));
            break;
        case 0xBF: // RES 7, A
            registers->a &= ~(1 << 7);
            break;
                
        case 0xC0: // SET 0, B
            registers->b = set(1 << 0, registers->b);
            break;
        case 0xC1: // SET 0, C
            registers->c = set(1 << 0, registers->c);
            break;
        case 0xC2: // SET 0, D
            registers->d = set(1 << 0, registers->d);
            break;
        case 0xC3: // SET 0, E
            registers->e = set(1 << 0, registers->e);
            break;
        case 0xC4: // SET 0, H
            registers->h = set(1 << 0, registers->h);
            break;
        case 0xC5: // SET 0, L
            registers->l = set(1 << 0, registers->l);
            break;
        case 0xC6: // SET 0, (HL)
            memory->write_byte(registers->hl, set(1 << 0, memory->read_byte(registers->hl)));
            break;
        case 0xC7: // SET 0, A
            registers->a = set(1 << 0, registers->a);
            break;

        case 0xC8: // SET 1, B
            registers->b = set(1 << 1, registers->b);
            break;
        case 0xC9: // SET 1, C
            registers->c = set(1 << 1, registers->c);
            break;
        case 0xCA: // SET 1, D
            registers->d = set(1 << 1, registers->d);
            break;
        case 0xCB: // SET 1, E
            registers->e = set(1 << 1, registers->e);
            break;
        case 0xCC: // SET 1, H
            registers->h = set(1 << 1, registers->h);
            break;
        case 0xCD: // SET 1, L
            registers->l = set(1 << 1, registers->l);
            break;
        case 0xCE: // SET 1, (HL)
            memory->write_byte(registers->hl, set(1 << 1, memory->read_byte(registers->hl)));
            break;
        case 0xCF: // SET 1, A
            registers->a = set(1 << 1, registers->a);
            break;

        case 0xD0: // SET 2, B
            registers->b = set(1 << 2, registers->b);
            break;
        case 0xD1: // SET 2, C
            registers->c = set(1 << 2, registers->c);
            break;
        case 0xD2: // SET 2, D
            registers->d = set(1 << 2, registers->d);
            break;
        case 0xD3: // SET 2, E
            registers->e = set(1 << 2, registers->e);
            break;
        case 0xD4: // SET 2, H
            registers->h = set(1 << 2, registers->h);
            break;
        case 0xD5: // SET 2, L
            registers->l = set(1 << 2, registers->l);
            break;
        case 0xD6: // SET 2, (HL)
            memory->write_byte(registers->hl, set(1 << 2, memory->read_byte(registers->hl)));
            break;
        case 0xD7: // SET 2, A
            registers->a = set(1 << 2, registers->a);
            break;

        case 0xD8: // SET 3, B
            registers->b = set(1 << 3, registers->b);
            break;
        case 0xD9: // SET 3, C
            registers->c = set(1 << 3, registers->c);
            break;
        case 0xDA: // SET 3, D
            registers->d = set(1 << 3, registers->d);
            break;
        case 0xDB: // SET 3, E
            registers->e = set(1 << 3, registers->e);
            break;
        case 0xDC: // SET 3, H
            registers->h = set(1 << 3, registers->h);
            break;
        case 0xDD: // SET 3, L
            registers->l = set(1 << 3, registers->l);
            break;
        case 0xDE: // SET 3, (HL)
            memory->write_byte(registers->hl, set(1 << 3, memory->read_byte(registers->hl)));
            break;
        case 0xDF: // SET 3, A
            registers->a = set(1 << 3, registers->a);
            break;


        case 0xE0: // SET 4, B
            registers->b = set(1 << 4, registers->b);
            break;
        case 0xE1: // SET 4, C
            registers->c = set(1 << 4, registers->c);
            break;
        case 0xE2: // SET 4, D
            registers->d = set(1 << 4, registers->d);
            break;
        case 0xE3: // SET 4, E
            registers->e = set(1 << 4, registers->e);
            break;
        case 0xE4: // SET 4, H
            registers->h = set(1 << 4, registers->h);
            break;
        case 0xE5: // SET 4, L
            registers->l = set(1 << 4, registers->l);
            break;
        case 0xE6: // SET 4, (HL)
            memory->write_byte(registers->hl, set(1 << 4, memory->read_byte(registers->hl)));
            break;
        case 0xE7: // SET 4, A
            registers->a = set(1 << 4, registers->a);
            break;


        case 0xE8: // SET 5, B
            registers->b = set(1 << 5, registers->b);
            break;
        case 0xE9: // SET 5, C
            registers->c = set(1 << 5, registers->c);
            break;
        case 0xEA: // SET 5, D
            registers->d = set(1 << 5, registers->d);
            break;
        case 0xEB: // SET 5, E
            registers->e = set(1 << 5, registers->e);
            break;
        case 0xEC: // SET 5, H
            registers->h = set(1 << 5, registers->h);
            break;
        case 0xED: // SET 5, L
            registers->l = set(1 << 5, registers->l);
            break;
        case 0xEE: // SET 5, (HL)
            memory->write_byte(registers->hl, set(1 << 5, memory->read_byte(registers->hl)));
            break;
        case 0xEF: // SET 5, A
            registers->a = set(1 << 5, registers->a);
            break;

        case 0xF0: // SET 6, B
            registers->b = set(1 << 6, registers->b);
            break;
        case 0xF1: // SET 6, C
            registers->c = set(1 << 6, registers->c);
            break;
        case 0xF2: // SET 6, D
            registers->d = set(1 << 6, registers->d);
            break;
        case 0xF3: // SET 6, E
            registers->e = set(1 << 6, registers->e);
            break;
        case 0xF4: // SET 6, H
            registers->h = set(1 << 6, registers->h);
            break;
        case 0xF5: // SET 6, L
            registers->l = set(1 << 6, registers->l);
            break;
        case 0xF6: // SET 6, (HL)
            memory->write_byte(registers->hl, set(1 << 6, memory->read_byte(registers->hl)));
            break;
        case 0xF7: // SET 6, A
            registers->a = set(1 << 6, registers->a);
            break;


        case 0xF8: // SET 7, B
            registers->b = set(1 << 7, registers->b);
            break;
        case 0xF9: // SET 7, C
            registers->c = set(1 << 7, registers->c);
            break;
        case 0xFA: // SET 7, D
            registers->d = set(1 << 7, registers->d);
            break;
        case 0xFB: // SET 7, E
            registers->e = set(1 << 7, registers->e);
            break;
        case 0xFC: // SET 7, H
            registers->h = set(1 << 7, registers->h);
            break;
        case 0xFD: // SET 7, L
            registers->l = set(1 << 7, registers->l);
            break;
        case 0xFE: // SET 7, (HL)
            memory->write_byte(registers->hl, set(1 << 7, memory->read_byte(registers->hl)));
            break;
        case 0xFF: // SET 7, A
            registers->a = set(1 << 7, registers->a);
            break;
        default:
            cout << "NOT IMPLEMENTED BC: " << hex << +opcode <<endl;
            exit(1);
            break;
    }
}

uint8_t InstructionSet::set(uint8_t bit, uint8_t value){
    value |= bit;
    return value;
}

void InstructionSet::bit(uint8_t bit, uint8_t value){
    if(value & bit) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->set_register_flag(FLAG_HALF_CARRY);
}

uint8_t InstructionSet::rlc(uint8_t value){
    int carry = (value & 0x80) >> 7;
    
    if(value & 0x80) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    value <<= 1;
    value += carry;
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);

    return value;
}

uint8_t InstructionSet::rrc(uint8_t value){
    int carry = value & 0x01;
    value >>= 1;
    
    if(carry){
        registers->set_register_flag(FLAG_CARRY);
        value |= 0x80;    
    } else registers->unset_register_flag(FLAG_CARRY);
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);

    return value;
}

uint8_t InstructionSet::rl(uint8_t value){
    int carry = registers->is_set_register_flag(FLAG_CARRY) ? 1 : 0;
    
    if(value & 0x80) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    value <<= 1;
    value += carry;
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);

    return value;
}

uint8_t InstructionSet::rr(uint8_t value){
    // value >>= 1;
    // if(registers->is_set_register_flag(FLAG_CARRY)) value |= 0x80;

    // if(value & 0x01) registers->set_register_flag(FLAG_CARRY);
    // else registers->unset_register_flag(FLAG_CARRY);
    uint8_t carry = registers->is_set_register_flag(FLAG_CARRY) ? 0x80 : 0x00;
    uint8_t result = value;

    if((value & 0x01) != 0) registers->set_register_flag(FLAG_CARRY); 
    else registers->unset_register_flag(FLAG_CARRY);
        
    result >>= 1;    
    result |= carry;    

    if(result) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);

    return result;
}

uint8_t InstructionSet::srl(uint8_t value){
    if(value & 0x01) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    value >>= 1;
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);

    return value;
}

uint8_t InstructionSet::sla(uint8_t value){
    if(value & 0x80) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    value <<= 1;
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);

    return value;
}

uint8_t InstructionSet::sra(uint8_t value){
    if(value & 0x01) registers->set_register_flag(FLAG_CARRY);
    else registers->unset_register_flag(FLAG_CARRY);
    
    value = (value & 0x80) | (value >> 1);
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);

    return value;
}

uint8_t InstructionSet::swap(uint8_t value) {
    value = ((value & 0xf) << 4) | ((value & 0xf0) >> 4);
    
    if(value) registers->unset_register_flag(FLAG_ZERO);
    else registers->set_register_flag(FLAG_ZERO);
    
    registers->unset_register_flag(FLAG_SUBTRACT);
    registers->unset_register_flag(FLAG_HALF_CARRY);
    registers->unset_register_flag(FLAG_CARRY);
    
    return value;
}


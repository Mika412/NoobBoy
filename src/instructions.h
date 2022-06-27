#pragma once

#include "registers.h"
#include "mmu.h"
#include "interrupt.h"


class InstructionSet{
    private:
        Registers *registers;
        MMU *mmu;
        Interrupts *interrupts;

        void write_short_stack(uint16_t value);
        uint16_t read_short_stack();

        const uint8_t instructionTicks[256] = {
            4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4,       // 0x0_
            4, 12, 8, 8, 4, 4, 8, 4,  12, 8, 8, 8, 4, 4, 8, 4,      // 0x1_
            0, 12, 8, 8, 4, 4, 8, 4,  0, 8, 8, 8, 4, 4, 8, 4,       // 0x2_
            0, 12, 8, 8, 12, 12, 12, 4,  0, 8, 8, 8, 4, 4, 8, 4,    // 0x3_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4,        // 0x4_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4,        // 0x5_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4,        // 0x6_
            8, 8, 8, 8, 8, 8, 4, 8,  4, 4, 4, 4, 4, 4, 8, 4,        // 0x7_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4,        // 0x8_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4,        // 0x9_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4,        // 0xa_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4,        // 0xb_
            0, 12, 0, 16, 0, 16, 8, 16,  0, 16, 0, 0, 0, 24, 8, 16, // 0xc_
            0, 12, 0, 0, 0, 16, 8, 16,  0, 16, 0, 0, 0, 0, 8, 16,   // 0xd_
            12, 12, 8, 0, 0, 16, 8, 16,  16, 4, 16, 0, 0, 0, 8, 16, // 0xe_
            12, 12, 8, 4, 0, 16, 8, 16,  12, 8, 16, 4, 0, 0, 8, 16  // 0xf_
        };
        const uint8_t extendedInstructionTicks[256] = {
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0x0_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0x1_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0x2_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0x3_
            8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8,     // 0x4_
            8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8,     // 0x5_
            8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8,     // 0x6_
            8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8,     // 0x7_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0x8_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0x9_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0xa_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0xb_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0xc_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0xd_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8,     // 0xe_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8      // 0xf_
        };
    public:

        InstructionSet(Registers* registers, Interrupts* interrupts, MMU* memory);
        void execute(uint8_t opcode);

        void ret(bool condition);
        void xor_(uint8_t value);
        void inc(uint8_t *value);
        void dec(uint8_t *value);
        void add(uint8_t *destination, uint8_t value);
        void add(uint16_t *destination, uint16_t value);
        void add(uint16_t *destination, int8_t value);
        void ldhl(int8_t value);
        void adc(uint8_t value);
        void sbc(uint8_t value);
        void sub(uint8_t value);
        void and_(uint8_t value);
        void or_(uint8_t value);
        void cp(uint8_t value);
        void call(bool condition);
        void jump(bool condition);
        void jump_add(bool condition);
        void cp_n(uint8_t value);


        // Extended instructions
        void extended_execute(uint8_t opcode);
        void bit(uint8_t bit, uint8_t value);
        void res(uint8_t bit, uint8_t *rgst);
        void set(uint8_t bit, uint8_t *rgst);
        void rl(uint8_t *value);
        void rlc(uint8_t *value);
        void rr(uint8_t *value);
        void rrc(uint8_t *value);
        void rra();
        void rla();
        void rlca();
        void sla(uint8_t *value);
        void sra(uint8_t *value);
        void srl(uint8_t *value);
        void swap(uint8_t *value);
};

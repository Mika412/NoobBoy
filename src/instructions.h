#pragma once

#include "Registers.h"
#include "Memory.h"


class InstructionSet{
    private:
        Registers *registers;
        MemoryBus *memory;
        bool *stopped;

    public:
        const unsigned char instructionTicks[256] = {
            4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4, // 0x0_
            4, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4, // 0x1_
            0, 12, 8, 8, 4, 4, 8, 4,  0, 8, 8, 8, 4, 4, 8, 4, // 0x2_
            0, 12, 8, 8, 12, 12, 12, 4,  0, 8, 8, 8, 4, 4, 8, 4, // 0x3_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4, // 0x4_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4, // 0x5_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4, // 0x6_
            8, 8, 8, 8, 8, 8, 4, 8,  4, 4, 4, 4, 4, 4, 8, 4, // 0x7_

            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4, // 0x8_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4, // 0x9_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4, // 0xA_
            4, 4, 4, 4, 4, 4, 8, 4,  4, 4, 4, 4, 4, 4, 8, 4, // 0xB_

            0, 12, 0, 16, 0, 16, 8, 16,  0, 16, 0, 4, 0, 24, 8, 16, // 0xc_
            0, 12, 0,  0, 0, 16, 8, 16,  0, 16, 0, 0, 0, 0, 8, 16, // 0xd_
            8, 12, 8,  0, 0, 16, 8, 16,  16, 4, 16, 0, 0, 0, 8, 16, // 0xe_
            8, 12, 8,  4, 0, 16, 8, 16,  12, 8, 16, 4, 0, 0, 8, 16  // 0xf_
        };
        /* const unsigned char instructionTicks[256] = {
            2, 6, 4, 4, 2, 2, 4, 4, 10, 4, 4, 4, 2, 2, 4, 4, // 0x0_
            2, 6, 4, 4, 2, 2, 4, 4,  4, 4, 4, 4, 2, 2, 4, 4, // 0x1_
            0, 6, 4, 4, 2, 2, 4, 2,  0, 4, 4, 4, 2, 2, 4, 2, // 0x2_
            4, 6, 4, 4, 6, 6, 6, 2,  0, 4, 4, 4, 2, 2, 4, 2, // 0x3_
            2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x4_
            2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x5_
            2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x6_
            4, 4, 4, 4, 4, 4, 2, 4,  2, 2, 2, 2, 2, 2, 4, 2, // 0x7_
            2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x8_
            2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x9_
            2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0xa_
            2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0xb_
            0, 6, 0, 6, 0, 8, 4, 8,  0, 2, 0, 0, 0, 6, 4, 8, // 0xc_
            0, 6, 0, 0, 0, 8, 4, 8,  0, 8, 0, 0, 0, 0, 4, 8, // 0xd_
            6, 6, 4, 0, 0, 8, 4, 8,  8, 2, 8, 0, 0, 0, 4, 8, // 0xe_
            6, 6, 4, 2, 0, 8, 4, 8,  6, 4, 8, 2, 0, 0, 4, 8  // 0xf_
        }; */
        const unsigned char extendedInstructionTicks[256] = {
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x0_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x1_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x2_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x3_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x4_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x5_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x6_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x7_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x8_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x9_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0xa_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0xb_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0xc_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0xd_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0xe_
            8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8  // 0xf_
        };

        InstructionSet(Registers* registers, MemoryBus* memory, bool *stopped);
        void execute(uint8_t opcode);
        void add(uint8_t* destination, uint8_t value);
        void sub(uint8_t value);
        void adc(uint8_t value);
        void sbc(uint8_t value);
        void or_(uint8_t value);
        void xor_(uint8_t value);
        uint8_t inc(uint8_t value);
        uint8_t dec(uint8_t value);
        void rla();
        void cp(uint8_t value);
        void cp_n(uint8_t value);
        void and_(uint8_t value);
        void and_n(uint8_t value);

        void add(uint16_t* destination, uint16_t value);

        // Extended instructions
        void extended_execute(uint8_t opcode);
        void bit(uint8_t bit, uint8_t value);
        uint8_t rlc(uint8_t value);
        uint8_t rrc(uint8_t value);
        uint8_t rl(uint8_t value);
        uint8_t rr(uint8_t value);
        uint8_t srl(uint8_t value);
        uint8_t sla(uint8_t value);
        uint8_t sra(uint8_t value);
        uint8_t swap(uint8_t value);
        
        uint8_t set(uint8_t destination, uint8_t value);
};

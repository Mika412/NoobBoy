#pragma once

#include <stdint.h>
#include <fstream>

enum Registers_8bit { A, B, C, D, E, F, H, L};
enum Registers_16bit { AF, BC, DE, HL};

struct FlagsRegister {
    bool zero = false;
    bool subtract = false;
    bool half_carry = false;
    bool carry = false;
};
enum RegisterFlagNames { FLAG_ZERO = 7, FLAG_SUBTRACT = 6, FLAG_HALF_CARRY = 5, FLAG_CARRY = 4};

class Registers {
    public:  
        uint16_t pc, sp;

        struct {
            union {
                struct {
                    uint8_t f;
                    uint8_t a;
            };
                uint16_t af;
            };
        }; 

        struct {
            union {
                struct {
                    uint8_t c;
                    uint8_t b;
            };
                uint16_t bc;
            };
        };  

        struct {
            union {
                struct {
                    uint8_t e;
                    uint8_t d;
            };
                uint16_t de;
            };
        };  

        struct {
            union {
                struct {
                    uint8_t l;
                    uint8_t h;
            };
                uint16_t hl;
            };
        };  

        unsigned long m = 0; // Quarter clock for last instr
        unsigned long t = 0; // Clock for last instr
        
        Registers();

        uint8_t get_val(Registers_8bit register_name);
        void set_val(Registers_8bit register_name, uint16_t value);
        void set_register_flags(FlagsRegister flags);
        void set_register_flag(RegisterFlagNames flag);
        void unset_register_flag(RegisterFlagNames flag);
        bool is_set_register_flag(RegisterFlagNames flag);
        FlagsRegister get_register_flags();
        uint8_t* get_register_variable(Registers_8bit reg);

        // Debug
        void print_flags();
        void print_registers();
};

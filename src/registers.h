#pragma once

#include <stdint.h>
#include <fstream>
#include <iostream>

enum RegisterFlags { FLAG_ZERO = (1 << 7), FLAG_SUBTRACT = (1 << 6), FLAG_HALF_CARRY = (1 << 5), FLAG_CARRY = (1 << 4)};

class Registers {
    public:  
        uint16_t pc; // Program Counter/Pointer
        uint16_t sp; // Stack pointer

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

        void set_flags(uint8_t flags, bool state);
        bool is_flag_set(uint8_t flag);

        // Debug
        void print_flags();
        void print_registers();
};

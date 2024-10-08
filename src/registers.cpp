#include "registers.h"

bool Registers::is_flag_set(uint8_t flag) { return this->f & flag; }

void Registers::set_flags(uint8_t flags, bool state) { this->f = state ? (this->f | flags) : (this->f & ~flags); }

void Registers::print_flags() {
    std::cout << "Z: " << std::hex << +is_flag_set(FLAG_ZERO) << " N: " << std::hex << +is_flag_set(FLAG_SUBTRACT)
              << " H: " << std::hex << +is_flag_set(FLAG_HALF_CARRY) << " C: " << std::hex << +is_flag_set(FLAG_CARRY)
              << std::endl;
}

void Registers::print_registers() {
    std::cout << "A: " << std::hex << +a << " F: " << std::hex << +f << std::endl;
    std::cout << "B: " << std::hex << +b << " C: " << std::hex << +c << std::endl;
    std::cout << "D: " << std::hex << +d << " E: " << std::hex << +e << std::endl;
    std::cout << "H: " << std::hex << +h << " L: " << std::hex << +l << std::endl;

    std::cout << "SP: " << std::hex << +sp << std::endl;
    std::cout << "PC: " << std::hex << +pc << std::endl;
    std::cout << "\n" << std::endl;
}

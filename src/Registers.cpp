#include <iostream>
#include "Registers.h"

void Registers::set_register_flags(uint8_t flags) {
    this->f |= flags;
}

void Registers::unset_register_flags(uint8_t flags) {
    this->f &= ~flags;
}

bool Registers::is_set_register_flag(uint8_t flag) {
    return (this->f & flag);
}

void Registers::print_flags(){
    std::cout << "Z: " << std::hex << +is_set_register_flag(FLAG_ZERO) << " N: " << std::hex << +is_set_register_flag(FLAG_SUBTRACT) << " H: " << std::hex << +is_set_register_flag(FLAG_HALF_CARRY) << " C: " << std::hex << +is_set_register_flag(FLAG_CARRY) << std::endl;
}

void Registers::print_registers(){
    std::cout << "A: " << std::hex << +a << " F: " << std::hex << +f << std::endl;
    std::cout << "B: " << std::hex << +b << " C: " << std::hex << +c << std::endl;
    std::cout << "D: " << std::hex << +d << " E: " << std::hex << +e << std::endl;
    std::cout << "H: " << std::hex << +h << " L: " << std::hex << +l << std::endl;

    std::cout << "SP: " << std::hex << +sp << std::endl;
    std::cout << "PC: " << std::hex << +pc << std::endl;
    std::cout << "\n" << std::endl;
}

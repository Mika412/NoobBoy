#include <iostream>

#include "Registers.h"

using namespace std;

Registers::Registers(){
    af = 0x0; 
    bc = 0x0; 
    de = 0x0; 
    hl = 0x0; 
    sp = 0x0; 
    pc = 0x0; 
}

uint8_t Registers::get_val(Registers_8bit register_name) {
  return *this->get_register_variable(register_name);
}

void Registers::set_val(Registers_8bit register_name, uint16_t value) {
  uint8_t *reg = this->get_register_variable(register_name);
  *reg = value;
}

void Registers::set_register_flags(FlagsRegister flags) {
  this->f = flags.zero << FLAG_ZERO | flags.subtract << FLAG_SUBTRACT |
            flags.half_carry << FLAG_HALF_CARRY | flags.carry << FLAG_CARRY;
}

void Registers::set_register_flag(RegisterFlagNames flag) {
  this->f |= (1 << flag);
}

void Registers::unset_register_flag(RegisterFlagNames flag) {
  this->f &= ~(1 << flag);
}

bool Registers::is_set_register_flag(RegisterFlagNames flag) {
  return (this->f >> flag) & 1;
}

FlagsRegister Registers::get_register_flags() {
  bool zero = ((this->f >> FLAG_ZERO) & 0b1) != 0;
  bool subtract = ((this->f >> FLAG_SUBTRACT) & 0b1) != 0;
  bool half_carry = ((this->f >> FLAG_HALF_CARRY) & 0b1) != 0;
  bool carry = ((this->f >> FLAG_CARRY) & 0b1) != 0;

  return FlagsRegister{zero, subtract, half_carry, carry};
}
uint8_t *Registers::get_register_variable(Registers_8bit reg) {
  switch (reg) {
  case A:
    return &this->a;
    break;
  case B:
    return &this->b;
    break;
  case C:
    return &this->c;
    break;
  case D:
    return &this->d;
    break;
  case E:
    return &this->e;
    break;
  case F:
    return &this->f;
    break;
  case H:
    return &this->h;
    break;
  case L:
    return &this->l;
    break;
  }
  return &this->a; // Default return pointer
}
// uint16_t get_register_comb_val(Registers_16bit comb_names){
//     uint8_t* first_reg =
//     this->get_register_variable(RegisterCompMap[comb_names].first); uint8_t*
//     second_reg =
//     this->get_register_variable(RegisterCompMap[comb_names].second);

//     return (*first_reg << 8) | *second_reg;
// }

// void set_register_comb_val(Registers_16bit comb_names, uint16_t value){
//     uint8_t* first_reg =
//     this->get_register_variable(RegisterCompMap[comb_names].first); uint8_t*
//     second_reg =
//     this->get_register_variable(RegisterCompMap[comb_names].second);

//     *first_reg = (value & 0xFF00) >> 8;
//     *second_reg = value & 0xFF;
// }

// private:

// uint16_t* get_register_variable(Registers_16bit reg){
//     switch (reg){
//         case AF:
//             return &this->af;
//             break;
// case BC:
//     return &this->bc;
//     break;
// case DE:
//     return &this->c;
//     break;
//         case HL:
//             return &this->hl;
//             break;
//     }
//     return &this->af; // Default return pointer
// }



void Registers::print_flags(){
    cout << "Z: " << hex << +is_set_register_flag(FLAG_ZERO) << " N: " << hex << +is_set_register_flag(FLAG_SUBTRACT) << " H: " << hex << +is_set_register_flag(FLAG_HALF_CARRY) << " C: " << hex << +is_set_register_flag(FLAG_CARRY) << endl;
}
void Registers::print_registers(){
    cout << "A: " << hex << +a << " F: " << hex << +f << endl;
    cout << "B: " << hex << +b << " C: " << hex << +c << endl;
    cout << "D: " << hex << +d << " E: " << hex << +e << endl;
    cout << "H: " << hex << +h << " L: " << hex << +l << endl;

    cout << "SP: " << hex << +sp << endl;
    cout << "PC: " << hex << +pc << endl;
    cout << "\n" << endl;
}

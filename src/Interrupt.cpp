#include "interrupt.h"
#include "mmu.h"


Interrupts::Interrupts(Registers *registers, MMU *mmu) {
    this->mmu = mmu;
    this->registers = registers;
}

void Interrupts::set_master_flag(bool state) {
    this->IME |= (int(state) << 0);
}

bool Interrupts::is_master_enabled() {
    return this->IME & 1U;
}

void Interrupts::set_interrupt_flag(uint8_t flag) {
    uint8_t IF_value = mmu->read_byte(0xFF0F);
    IF_value |= flag;
    return mmu->write_byte(0xFF0F, IF_value);
}

void Interrupts::unset_interrupt_flag(uint8_t flag) {
    uint8_t IF_value = mmu->read_byte(0xFF0F);
    IF_value &= ~flag;
    return mmu->write_byte(0xFF0F, IF_value);
}

bool Interrupts::is_interrupt_enabled(uint8_t flag) {
    return (mmu->read_byte(0xFFFF) & flag);
}

bool Interrupts::is_interrupt_flag_set(uint8_t flag) {
    return (mmu->read_byte(0xFF0F) & flag);
}

void Interrupts::check() {
    if (!this->IME)
        return;

    if(this->jump_vblank){
        this->mmu->write_short_stack(&registers->sp, this->registers->pc);
        this->registers->pc = 0x40;
        this->unset_interrupt_flag(INTERRUPT_VBLANK);
        this->set_master_flag(false);
        this->jump_vblank = false;
        // mmu->is_halted = false;
        // std::cout << "INTERRUPTING LIKE CRAZY" << std::endl;
    }
    if(this->jump_timer){
        this->mmu->write_short_stack(&registers->sp, this->registers->pc);
        this->registers->pc = 0x50;
        this->set_master_flag(false);
        this->unset_interrupt_flag(INTERRUPT_TIMER);
        this->jump_timer = false;
        // std::cout << "INTERRUPTING LIKE CRAZY" << std::endl;
        // mmu->is_halted = false;
    }

    if(this->jump_joypad){
        this->mmu->write_short_stack(&registers->sp, this->registers->pc);
        this->registers->pc = 0x60;
        this->set_master_flag(false);
        this->unset_interrupt_flag(INTERRUPT_JOYPAD);
        this->jump_joypad = false;
        // std::cout << "INTERRUPTING LIKE CRAZY JOYPAD" << std::endl;
        // mmu->is_halted = false;
    }
        // if(fire & (1 << 4)){ // JOYPAD
        //     std::cout << "SHOULD JOYPAD" << std::endl;
        //     memory->interruptFlags.IF &= ~(1 << 4);
        //     memory->interruptFlags.IME = 0;
        //     memory->write_short_stack(registers->pc);
        //     registers->pc = 0x60;
        //     registers->t += 12;
        //     return true;
        // }
    
    // if ((this->mmu->read_byte(0xFFFF) & this->mmu->read_byte(0xFF0F)) && !this->triggered_jump){
    //     triggered_jump = true;
    //     return;
    // }
 
    // std::cout << this->is_interrupt_enabled(INTERRUPT_VBLANK) << " " << this->is_interrupt_flag_set(INTERRUPT_VBLANK) << "\n";
    // Check if VBLANK
    if(this->is_interrupt_enabled(INTERRUPT_VBLANK) && this->is_interrupt_flag_set(INTERRUPT_VBLANK)) {
        this->jump_vblank = true;
        // std::cout << "GPU State: " << ppu->gpu_mode << " Clock: " << ppu->clock << std::endl;
    }
    // // Check if TIMER
    if(this->is_interrupt_enabled(INTERRUPT_TIMER) && this->is_interrupt_flag_set(INTERRUPT_TIMER)) {
        this->jump_timer = true;
    }

    // // Check if JOYPAD
    if(this->is_interrupt_enabled(INTERRUPT_JOYPAD) && this->is_interrupt_flag_set(INTERRUPT_JOYPAD)) {
        this->jump_joypad = true;
    }

    triggered_jump = false;
}

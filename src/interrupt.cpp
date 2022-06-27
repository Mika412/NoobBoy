#include "interrupt.h"

Interrupts::Interrupts(Registers *registers, MMU *mmu) {
    this->mmu = mmu;
    this->registers = registers;
}

void Interrupts::set_master_flag(bool state) {
    this->IME = (int(state) << 0);
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

bool Interrupts::check() {
    if (mmu->read_byte(0xFF0F) & 0x0F)
        this->mmu->is_halted = false;

    if (!this->IME)
        return false;

    // Check if VBLANK
    if(this->is_interrupt_enabled(INTERRUPT_VBLANK) && this->is_interrupt_flag_set(INTERRUPT_VBLANK)) {
        this->trigger_interrupt(INTERRUPT_VBLANK, 0x40);
        return true;
    }

    // Check if LCD
    if(this->is_interrupt_enabled(INTERRUPT_LCD) && this->is_interrupt_flag_set(INTERRUPT_LCD)) {
        this->trigger_interrupt(INTERRUPT_LCD, 0x48);
        return true;
    }

    // Check if TIMER
    if(this->is_interrupt_enabled(INTERRUPT_TIMER) && this->is_interrupt_flag_set(INTERRUPT_TIMER)) {
        this->trigger_interrupt(INTERRUPT_TIMER, 0x50);
        return true;
    }

    // Check if JOYPAD
    if(this->is_interrupt_enabled(INTERRUPT_JOYPAD) && this->is_interrupt_flag_set(INTERRUPT_JOYPAD)) {
        this->trigger_interrupt(INTERRUPT_JOYPAD, 0x60);
        return true;
    }


    return false;
}

void Interrupts::trigger_interrupt(InterruptFlags interrupt, uint8_t jump_pc) {
    this->mmu->write_short_stack(&registers->sp, this->registers->pc);
    this->registers->pc = jump_pc;
    this->set_master_flag(false);
    this->unset_interrupt_flag(interrupt);
    mmu->is_halted = false;

    mmu->clock.t_instr = 20;
}

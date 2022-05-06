#pragma once

// #include "cpu.h"
#include "mmu.h"

enum InterruptFlags { INTERRUPT_VBLANK = (1 << 0), INTERRUPT_LCD = (1 << 1), INTERRUPT_TIMER = (1 << 2), INTERRUPT_SERIAL = (1 << 3), INTERRUPT_JOYPAD = (1 << 4)};

class Interrupts {
    Registers *registers;
    MMU *mmu;

    public:
        bool IME;

        bool triggered_jump = false;
        bool jump_vblank = false;
        bool jump_timer = false;
        bool jump_joypad = false;

        Interrupts(Registers* registers, MMU* mmu);

        void check();
        void set_master_flag(bool state);
        bool is_master_enabled();
        void set_interrupt_flag(uint8_t flag);
        void unset_interrupt_flag(uint8_t flag);
        bool is_interrupt_enabled(uint8_t flag);
        bool is_interrupt_flag_set(uint8_t flag);
};

#pragma once
#include "SDL2/SDL.h"

#include "mmu.h"
#include "interrupt.h"
#include "status.h"

enum ButtonFlags { 
    JOYPAD_A        = (1 << 0),
    JOYPAD_B        = (1 << 1),
    JOYPAD_SELECT   = (1 << 2), 
    JOYPAD_START    = (1 << 3), 

    JOYPAD_RIGHT    = (1 << 4), 
    JOYPAD_LEFT     = (1 << 5), 
    JOYPAD_UP       = (1 << 6), 
    JOYPAD_DOWN     = (1 << 7), 
};

class Joypad {
    Status *status;
    MMU *mmu;
    Interrupts *interrupts;

    uint8_t joypad_state;
    int joypad_cycles = 0;

    void update_joypad_memory();
    public:

        Joypad(Status* status, Interrupts* interrupts, MMU* mmu);

        void key_press(ButtonFlags key);
        void key_release(ButtonFlags key);
        void check(int last_instr_cycles);
};

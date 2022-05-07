#pragma once

#include "mmu.h"
#include "interrupt.h"

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
    MMU *mmu;
    Interrupts *interrupts;

    public:

        Joypad(Interrupts* interrupts, MMU* mmu);

        void key_press(ButtonFlags key);
        void key_release(ButtonFlags key);
};

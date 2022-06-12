#include "joypad.h"

Joypad::Joypad(Interrupts* interrupts, MMU* mmu){
    this->interrupts = interrupts;
    this->mmu = mmu;
}

void Joypad::key_press(ButtonFlags key){
    mmu->joypad &= ~(0xFF & key);
    interrupts->set_interrupt_flag(INTERRUPT_JOYPAD);
}

void Joypad::key_release(ButtonFlags key){
    mmu->joypad |= 0xFF & key;
    interrupts->set_interrupt_flag(INTERRUPT_JOYPAD);
}

void Joypad::check(int last_instr_cycles){
    joypad_cycles += last_instr_cycles;
    if(joypad_cycles < 65536)
        return; 

    joypad_cycles -= 65536;

    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_KEYUP:
            switch(event.key.keysym.sym){
                case SDLK_RIGHT:  this->key_release(JOYPAD_RIGHT); break;
                case SDLK_LEFT:   this->key_release(JOYPAD_LEFT); break;
                case SDLK_UP:     this->key_release(JOYPAD_UP); break;
                case SDLK_DOWN:   this->key_release(JOYPAD_DOWN); break;
                case SDLK_z:      this->key_release(JOYPAD_A); break;
                case SDLK_x:      this->key_release(JOYPAD_B); break;
                case SDLK_SPACE:  this->key_release(JOYPAD_START); break;
                case SDLK_RETURN: this->key_release(JOYPAD_SELECT); break;
            }
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
                case SDLK_RIGHT:  this->key_press(JOYPAD_RIGHT); break;
                case SDLK_LEFT:   this->key_press(JOYPAD_LEFT); break;
                case SDLK_UP:     this->key_press(JOYPAD_UP); break;
                case SDLK_DOWN:   this->key_press(JOYPAD_DOWN); break;
                case SDLK_z:      this->key_press(JOYPAD_A); break;
                case SDLK_x:      this->key_press(JOYPAD_B); break;
                case SDLK_SPACE:  this->key_press(JOYPAD_START); break;
                case SDLK_RETURN: this->key_press(JOYPAD_SELECT); break;
                // case SDLK_ESCAPE: isRunning = false; break;
            }
            break;
        case SDL_QUIT:
            // isRunning = false;
            exit(0);
            break;
    }
    // if(debug){
    //     switch (event.type) {
    //         case SDL_KEYUP:
    //             switch(event.key.keysym.sym){
    //                 case SDLK_s:  doStep = true; break;
    //                 case SDLK_p:  isPaused = !isPaused; break;
    //             }
    //             break;
    //     }
    // }
}

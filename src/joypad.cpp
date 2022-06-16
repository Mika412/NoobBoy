#include "joypad.h"

Joypad::Joypad(Status* status, Interrupts* interrupts, MMU* mmu){
    this->interrupts = interrupts;
    this->mmu = mmu;
    this->status = status;
}

void Joypad::key_press(ButtonFlags key){
    this->joypad_state = mmu->joypad & ~(0xFF & key);
}

void Joypad::key_release(ButtonFlags key){
    this->joypad_state = mmu->joypad | (0xFF & key);
}
void Joypad::update_joypad_memory(){
    if(this->joypad_state) {
        mmu->joypad = this->joypad_state;
        this->joypad_state = 0;
        interrupts->set_interrupt_flag(INTERRUPT_JOYPAD);
    }
}

void Joypad::check(int last_instr_cycles){
    joypad_cycles += last_instr_cycles;
    if(!status->isPaused){
        if(joypad_cycles < 65536)
            return;
        joypad_cycles -= 65536;
    }

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
                case SDLK_s:  if(status->debug) status->doStep = true; break;
                case SDLK_p:  if(status->debug) status->isPaused = !this->status->isPaused; break;
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
                case SDLK_ESCAPE: this->status->isRunning = false; break;
                case SDLK_1:      this->status->colorMode = RETRO; break;
                case SDLK_2:      this->status->colorMode = NORMAL; break;
            }
            break;
        case SDL_QUIT:
            this->status->isRunning = false;
            exit(0);
            break;
    }
    update_joypad_memory();
}

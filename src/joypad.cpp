#include "joypad.h"

Joypad::Joypad(Status* status, Interrupts* interrupts, MMU* mmu) {
    this->interrupts = interrupts;
    this->mmu = mmu;
    this->status = status;
}

void Joypad::key_press(ButtonFlags key) { this->joypad_state = mmu->joypad & ~(0xFF & key); }

void Joypad::key_release(ButtonFlags key) { this->joypad_state = mmu->joypad | (0xFF & key); }
void Joypad::update_joypad_memory() {
    if (this->joypad_state) {
        mmu->joypad = this->joypad_state;
        this->joypad_state = 0;
        interrupts->set_interrupt_flag(INTERRUPT_JOYPAD);
    }
}

void Joypad::check(int last_instr_cycles) {
    joypad_cycles += last_instr_cycles;
    if (!status->isPaused) {
        if (joypad_cycles < 65536)
            return;
        joypad_cycles -= 65536;
    }

    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_EVENT_KEY_UP:
            switch (event.key.key) {
                case SDLK_RIGHT:
                    key_release(JOYPAD_RIGHT);
                    break;
                case SDLK_LEFT:
                    key_release(JOYPAD_LEFT);
                    break;
                case SDLK_UP:
                    key_release(JOYPAD_UP);
                    break;
                case SDLK_DOWN:
                    key_release(JOYPAD_DOWN);
                    break;
                case SDLK_Z:
                    key_release(JOYPAD_A);
                    break;
                case SDLK_X:
                    key_release(JOYPAD_B);
                    break;
                case SDLK_SPACE:
                    key_release(JOYPAD_START);
                    break;
                case SDLK_RETURN:
                    key_release(JOYPAD_SELECT);
                    break;
                case SDLK_C:
                    status->colorMode = (1 + status->colorMode) % 3;
                    break;
                case SDLK_P:
                    if (status->debug)
                        status->isPaused = !status->isPaused;
                    break;
                case SDLK_S:
                    mmu->save_game_state();
                    break;
                case SDLK_N:
                    if (status->debug)
                        status->doStep = true;
                    break;
                case SDLK_M:
                    status->soundEnabled = !status->soundEnabled;
                    break;
            }
            break;
        case SDL_EVENT_KEY_DOWN:
            switch (event.key.key) {
                case SDLK_RIGHT:
                    key_press(JOYPAD_RIGHT);
                    break;
                case SDLK_LEFT:
                    key_press(JOYPAD_LEFT);
                    break;
                case SDLK_UP:
                    key_press(JOYPAD_UP);
                    break;
                case SDLK_DOWN:
                    key_press(JOYPAD_DOWN);
                    break;
                case SDLK_Z:
                    key_press(JOYPAD_A);
                    break;
                case SDLK_X:
                    key_press(JOYPAD_B);
                    break;
                case SDLK_SPACE:
                    key_press(JOYPAD_START);
                    break;
                case SDLK_RETURN:
                    key_press(JOYPAD_SELECT);
                    break;
                case SDLK_ESCAPE:
                    status->isRunning = false;
                    break;
            }
            break;
        case SDL_EVENT_QUIT:
            this->status->isRunning = false;
            exit(0);
            break;
    }
    update_joypad_memory();
}

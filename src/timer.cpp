#include "timer.h"

Timer::Timer(MMU* mmu, Interrupts* interrupts){
    this->mmu = mmu;
    this->interrupts = interrupts;
}

void Timer::inc(){
    mmu->clock.t += mmu->clock.t_instr;
    mmu->clock.t %= 4194304;

    div += mmu->clock.t_instr;

    while(div >= 256){
        div -= 256;
        mmu->timer.div++;
    }

    check();
}

void Timer::check(){
    if(mmu->timer.tac & 0x04){
        tima += mmu->clock.t_instr;

        int threshold = 0;
        switch(mmu->timer.tac & 0x03){
            case 0: 
                threshold = 1024;
                break;
            case 1: 
                threshold = 16;
                break;
            case 2: 
                threshold = 64;
                break;
            case 3: 
                threshold = 256;
                break;
        }
        while(tima >= threshold){
            tima -= threshold;
            if(mmu->timer.tima == 0xFF){
                mmu->timer.tima = mmu->read_byte(0xFF06);
                interrupts->set_interrupt_flag(INTERRUPT_TIMER);
            }else{
                mmu->timer.tima++;
            }
        }
    }
}

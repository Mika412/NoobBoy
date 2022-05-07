#include <iostream>

#include "timer.h"

Timer::Timer(Registers *registers, MMU* memory, Interrupts* interrupts){
    this->registers = registers;
    this->memory = memory;
    this->interrupts = interrupts;
}

void Timer::inc(){
    div += this->registers->m;
        std::cout << "SUB " << +(memory->timer.div) << std::endl;
    if(div >= 4){
        // main++;
        div -= 4;
        memory->timer.div++;
        // // div++;
        // if(div == 16){
        //     std::cout << "SUB " << +(sub) << std::endl;
        //     memory->timer.div = (memory->timer.div + 1);
        //     // memory->timer.div = (memory->timer.div + 1) & 255;
        //     div = 0;
        // }
    }

    check();
}

void Timer::check(){
    if((memory->timer.tac >> 2) & 0x1){
        exit(1);
        int threshold = 1000; // TODO: Replace this
        switch(memory->timer.tac & 3){
            case 0: 
                threshold = 64;
                break;
            case 1: 
                threshold = 1;
                break;
            case 2: 
                threshold = 4;
                break;
            case 3: 
                threshold = 16;
                break;
        }
        if(main >= threshold)
            step();
    }
}

void Timer::step(){
    main = 0;
    memory->timer.tima++;

    if(memory->timer.tima > 255){
        memory->timer.tima = memory->timer.tma;
        interrupts->set_interrupt_flag(INTERRUPT_TIMER);
        // memory->interruptFlags.IF |= 4;
    }
}

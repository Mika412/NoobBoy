#include <iostream>

#include "timer.h"

Timer::Timer(Registers *registers, MemoryBus* memory){
    this->registers = registers;
    this->memory = memory;
}

void Timer::inc(){
    sub += this->registers->m;
    if(sub >= 4){
        main++;
        sub -= 4;

        div++;
        if(div == 16){
            memory->timer.div = (memory->timer.div + 1) & 255;
            div = 0;
        }
    }

    check();
}

void Timer::check(){
    if(memory->timer.tac & 4){
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
        
        // memory->interruptFlags.IF |= 4;
    }
}

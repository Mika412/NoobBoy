#include <iostream>

#include "gpu.h"

// Remove this later
// #include <SDL2/SDL_image.h>
// #include <SDL2/SDL_timer.h>


GPU::GPU(Registers *registers, Interrupts* interrupts, MMU* mmu){
    this->registers = registers;
    this->mmu = mmu;
    this->interrupts = interrupts; 
    control = &mmu->memory[0xff40];
    scrollY = &mmu->memory[0xff42];
    scrollX = &mmu->memory[0xff43];
    scanline = &mmu->memory[0xff44];
}

void GPU::step(){
    // std::cout << "SCANLINE: " << +this->scanline << std::endl;
    // std::cout << "MODE " << mode << std::endl;
    // std::cout << "Incorrect cycles: " << this->cpu->cycles << std::endl;
    modeclock += registers->t;
    // if(!this->control){
    //     return;
    // }
    switch (mode) {
        case 0: // HBLANK
            if(modeclock >= 204){
                *this->scanline+=1;

                if(*this->scanline == 143){
                    if(this->interrupts->is_interrupt_enabled(INTERRUPT_VBLANK)){
                        this->interrupts->set_interrupt_flag(INTERRUPT_VBLANK);
                    }
                    mode = 1;
                }else{
                    mode = 2;
                }
                modeclock -= 204;
                // render_scan_lines();
            }
            break;
        case 1: // VBLANK
            if(modeclock >= 456) {
                // modeclock = 0;
                *this->scanline+=1;

                if(*this->scanline > 153){
                    //Restart scanning modes
                    *this->scanline = 0;
                    mode = 2;
                    mmu->renderFlag.viewport = true;



                }
                modeclock -=456;
            }
            break;
        case 2: // OAM
            if(modeclock >= 80){
                modeclock -=80;
                mode = 3;
            }
            break;
        case 3: // VRAM
            if(modeclock >= 172){
                mode = 0;
                render_scan_lines();

                modeclock -= 172;
            }
            break;
            
    }
}
void GPU::render_scan_lines(){
    // Tiles
    uint16_t address = 0x9800;
    if((*this->control >> 3) & 1)
        address += 0x400;

    address += (((*this->scanline + *this->scrollY) & 255) >> 3) << 5;
    address += (*this->scrollX >> 3);

    int x = *this->scrollX & 7;
    int y = (*this->scanline + *this->scrollY) & 7;

    int pixelOffset = *this->scanline * 160;
    for(uint16_t tile_address = address; tile_address < address + 20; tile_address++){
        int tile = this->mmu->read_byte(tile_address);
        
        // Check if background tiles
        if(!(*this->control >> 6 & 0x1) && tile < 128)
            tile += 256;

        for(; x < 8; x++){
            unsigned char colour = mmu->tiles[tile][y][x];
            framebuffer[pixelOffset++] = palette[colour];
        }
        x=0;
    }

    // Sprites
}

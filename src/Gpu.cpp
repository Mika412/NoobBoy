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
    bool scanlineRow[160];
    int rowPixel = 0;

    this->render_scan_line_background(scanlineRow);
    this->render_scan_line_window(scanlineRow);
    this->render_scan_line_sprites(scanlineRow);
}

void GPU::render_scan_line_background(bool scanlineRow[]){
    int rowPixel = 0;

    // Tiles
    uint16_t address = 0x9800;
    // if((*this->control >> 3) & 1)
    //     address += 0x400;
    
    address += (*this->scanline / 8) * 32;
    address += (*this->scrollX >> 3);

    int x = *this->scrollX & 7;
    int y = *this->scanline & 7;
    int pixelOffset = *this->scanline * 160;

    for(uint16_t tile_address = address; tile_address < address + 20; tile_address++){
        // if(tile_address % 0x20 != 0){
        //     exit(1);
        // }
        int tile = this->mmu->read_byte(tile_address);

        // Check if background tiles
        // if(!(*this->control >> 5 & 0x1) && tile < 128)
        //     tile += 256;
        if(tile < 128)
            tile += 256;

        for(; x < 8; x++){
            int colour = mmu->tiles[tile][y][x];
            framebuffer[pixelOffset++] = palette[colour];
            scanlineRow[rowPixel++] = 1;
        }
        x=0;
    }
}

void GPU::render_scan_line_window(bool scanlineRow[]){
    if(!(*this->control >> 5 & 0x1)){
        return;
    }
    if(mmu->read_byte(0xFF4A) > *this->scanline){
        return; 
    }
    // bool scanlineRow[160];
    int rowPixel = 0;

    // Tiles
    uint16_t address = 0x9C00;
    // if((*this->control >> 3) & 1)
    //     address += 0x400;
    
    std::cout << +*this->scanline << std::endl;
    address += ((*this->scanline - mmu->read_byte(0xFF4A)) / 8) * 32;
    int y = (*this->scanline - mmu->read_byte(0xFF4A)) & 7;
    int x = 0;

    int pixelOffset = *this->scanline * 160;
    pixelOffset += mmu->read_byte(0xFF4B) - 7;

    for(uint16_t tile_address = address; tile_address < address + 20; tile_address++){
        int tile = this->mmu->read_byte(tile_address);

        // Check if background tiles
        // if(!(*this->control >> 5 & 0x1) && tile < 128)
        //     tile += 256;
        if(tile < 128)
            tile += 256;

        for(; x < 8; x++){
            int colour = mmu->tiles[tile][y][x];
            framebuffer[pixelOffset++] = palette[colour];
            scanlineRow[rowPixel++] = 1;
        }
        x=0;
    }

}

void GPU::render_scan_line_sprites(bool scanlineRow[]){
    int rowPixel = 0;
    // Sprites
    if(!((*control >> 1) & 1)) 
        return;

    for(auto sprite : mmu->sprites){
        if(sprite.y <= *scanline && (sprite.y + 8) > *scanline) {
            int pixelOffset = *this->scanline * 160 + sprite.x;

            uint8_t y = *scanline % sprite.y;
            if(sprite.options.yFlip) y = 7 - y;

            for(int x = 0; x < 8; x++){
                if(sprite.x + x >= 0 && sprite.x + x < 160) {
                    // if(!scanlineRow[sprite.x + x] || sprite.options.renderPriority)
                    //     continue;

                    uint8_t xF = sprite.options.xFlip ? 7 - x : x;
                    uint8_t colour = mmu->tiles[sprite.tile][y][xF];

                    if(colour){
                        framebuffer[pixelOffset] = palette[colour];
                    }
                    pixelOffset++;
                    scanlineRow[sprite.x + x] = 1;
                }
            }
        }
    }
}

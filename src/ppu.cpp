#include <iostream>

#include "ppu.h"

PPU::PPU(Registers *registers, Interrupts* interrupts, MMU* mmu){
    this->registers = registers;
    this->mmu = mmu;
    this->interrupts = interrupts; 
    // TODO: Rewrite this to use mmu->read_byte(0xff40)
    control = (Control*)&mmu->memory[0xff40];
    scrollY = &mmu->memory[0xff42];
    scrollX = &mmu->memory[0xff43];
    scanline = &mmu->memory[0xff44];
}

void PPU::step(){

    for (mmu->timer.div += mmu->clock.t - mmu->clock.t_prev; mmu->clock.t_prev++ != mmu->clock.t;)
      if (control->lcdEnable) {
        if (++modeclock == 456) {

          if (*scanline < 144)
            render_scan_lines();
          if (*scanline == 143) {
            this->interrupts->set_interrupt_flag(INTERRUPT_VBLANK);
            can_render = true;
            mmu->renderFlag.viewport = true;
          }

            if(*this->scanline == mmu->read_byte(0xff45)){// && *this->scanline > 0){
                mmu->write_byte(0xff41, mmu->read_byte(0xff41) | (1 << 2));
            }else{
                mmu->write_byte(0xff41, mmu->read_byte(0xff41) & ~(1 << 2));
            }
          *scanline = (*scanline + 1) % 154;
          modeclock = 0;
        }
    } else{
        *scanline = modeclock = 0;
    }
}

void PPU::step2(){
    // std::cout << "SCANLINE: " << +this->scanline << std::endl;
    // std::cout << "MODE " << mode << std::endl;
    // std::cout << "Incorrect cycles: " << this->cpu->cycles << std::endl;
    modeclock += mmu->clock.t;
    // modeclock += registers->t;
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
                        can_render = true;
                    }
                    can_render = true;
                    mode = 1;
                }else{
                    mode = 2;
                }
                modeclock -= 204;
                // render_scan_lines();

                // std::cout << "SCANLINE: "<< +(*this->scanline) << std::endl;
                // if(*this->scanline == mmu->read_byte(0xff45) && *this->scanline > 0){
                //     mmu->write_byte(0xFF41, mmu->read_byte(0xFF41) | (1 << 2));
                // }else{
                //     mmu->write_byte(0xFF41, mmu->read_byte(0xFF41) & ~(1 << 2));
                // }
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
                if(*this->scanline == mmu->read_byte(0xff45) && *this->scanline > 0){
                    mmu->write_byte(0xff41, mmu->read_byte(0xff41) | (1 << 2));
                }else{
                    mmu->write_byte(0xff41, mmu->read_byte(0xff41) & ~(1 << 2));
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

void PPU::render_scan_lines(){
    if(!this->control->lcdEnable)
        return;

    this->render_scan_line_background();
    this->render_scan_line_window();
    this->render_scan_line_sprites();
}

void PPU::render_scan_line_background(){
    uint16_t address = 0x9800;

    if(this->control->bgDisplaySelect)
        address += 0x400;

    address += (*this->scanline / 8) * 32;
    address += (*this->scrollX >> 3);

    int x = *this->scrollX & 7;
    int y = *this->scanline & 7;
    int pixelOffset = *this->scanline * 160;

    for(uint16_t tile_address = address; tile_address < address + 20; tile_address++){
        int tile = this->mmu->read_byte(tile_address);

        if(tile < 128)
            tile += 256;

        for(; x < 8; x++){
            int colour = mmu->tiles[tile][y][x];
            framebuffer[pixelOffset++] = palette[colour];
        }
        x=0;
    }
}

void PPU::render_scan_line_window(){
    if(!this->control->windowEnable){
        return;
    }

    if(mmu->read_byte(0xFF4A) > *this->scanline){
        return; 
    }

    uint16_t address = 0x9800;
    if(this->control->windowDisplaySelect)
        address += 0x400;
    
    address += ((*this->scanline - mmu->read_byte(0xFF4A)) / 8) * 32;
    int y = (*this->scanline - mmu->read_byte(0xFF4A)) & 7;
    int x = 0;

    int pixelOffset = *this->scanline * 160;
    pixelOffset += mmu->read_byte(0xFF4B) - 7;

    for(uint16_t tile_address = address; tile_address < address + 20; tile_address++){
        int tile = this->mmu->read_byte(tile_address);

        if(tile < 128)
            tile += 256;

        for(; x < 8; x++){
            int colour = mmu->tiles[tile][y][x];
            framebuffer[pixelOffset++] = palette[colour];
        }
        x=0;
    }

}

void PPU::render_scan_line_sprites(){
    if(!this->control->spriteDisplayEnable) 
        return;

    for(auto sprite : mmu->sprites){
        if(sprite.y <= *scanline && (sprite.y + 8) > *scanline) {
            int pixelOffset = *this->scanline * 160 + sprite.x;
            
            // Flip vertically
            uint8_t y = *scanline % sprite.y;
            if(sprite.options.yFlip) y = 7 - y;

            for(int x = 0; x < 8; x++){
                if(sprite.x + x >= 0 && sprite.x + x < 160) {
                    // Flip horizontally
                    uint8_t xF = sprite.options.xFlip ? 7 - x : x;
                    uint8_t colour = mmu->tiles[sprite.tile][y][xF];

                    if(colour)
                        framebuffer[pixelOffset] = palette[colour];
                    pixelOffset++;
                }
            }
        }
    }
}

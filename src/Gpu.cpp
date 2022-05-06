#include <iostream>

#include "gpu.h"

// Remove this later
// #include <SDL2/SDL_image.h>
// #include <SDL2/SDL_timer.h>


GPU::GPU(Registers *registers, Interrupts* interrupts, MemoryBus* memory){
    this->registers = registers;
    this->memory = memory;
    this->interrupts = interrupts; 
}

void GPU::step(){
    // std::cout << "SCANLINE: " << +memory->gpu.scanline << std::endl;
    // std::cout << "MODE " << mode << std::endl;
    // std::cout << "Incorrect cycles: " << this->cpu->cycles << std::endl;
    modeclock += registers->t;
    // if(!memory->gpu.control){
    //     return;
    // }
    switch (mode) {
        case 0: // HBLANK
            if(modeclock >= 204){
                memory->gpu.scanline++;

                if(memory->gpu.scanline == 143){
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
                memory->gpu.scanline++;

                if(memory->gpu.scanline > 153){
                    //Restart scanning modes
                    memory->gpu.scanline = 0;
                    mode = 2;
                    memory->renderFlag.viewport = true;



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
    int mapOffset = (memory->gpu.control & (1 << 3)) ? 0x1c00 : 0x1800;
    /* mapOffset += (((memory->gpu.scanline + memory->gpu.scrollY) & 255) >> 3);         */
    mapOffset += (((memory->gpu.scanline + memory->gpu.scrollY) & 255) >> 3) << 5;        
    // std::cout << "SCROLL " << +memory->gpu.scrollY << std::endl;

    int lineOffset = (memory->gpu.scrollX >> 3);

    int x = memory->gpu.scrollX & 7;
    int y = (memory->gpu.scanline + memory->gpu.scrollY) & 7;

    int pixelOffset = memory->gpu.scanline * 160;
    int tst = mapOffset + lineOffset;

    unsigned char tile = (unsigned char)memory->vram[mapOffset + lineOffset];

    for(int i = 0; i < 160; i++) {
        unsigned char colour = memory->tiles[tile][y][x];

        framebuffer[pixelOffset].r = palette[colour].r;
        framebuffer[pixelOffset].g = palette[colour].g;
        framebuffer[pixelOffset].b = palette[colour].b; 
        framebuffer[pixelOffset].a = palette[colour].a;

        pixelOffset++;
        x++;
        if(x == 8) {
            x = 0;
            lineOffset = (lineOffset + 1) & 31;
            tile = memory->vram[mapOffset + lineOffset];
            //if((gpu.control & GPU_CONTROL_TILESET) && tile < 128) tile += 256;
        }
    }
}

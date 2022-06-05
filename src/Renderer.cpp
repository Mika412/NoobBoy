#include "renderer.h"
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iomanip>

Renderer::Renderer(CPU *cpu, GPU *gpu, Registers *registers, MMU *memory){
    this->cpu = cpu;
    this->gpu = gpu;
    this->registers = registers;
    this->memory = memory;
    this->mmu = memory;

    SDL_Init(SDL_INIT_VIDEO);
    if(debug){
        window_width = debug_texture_width;
        window_height = debug_texture_height;
    }
    SDL_CreateWindowAndRenderer(this->window_width*2, this->window_height*2, 0, &this->window, &this->renderer);
    SDL_RenderSetLogicalSize(this->renderer, this->window_width, this->window_height);
    SDL_SetWindowResizable(this->window, SDL_TRUE);
    //
    std::vector< unsigned char > viewport_pixels(this->viewport_width * this->viewport_height * 4, 0xFF );
    this->viewport_pixels = viewport_pixels;

    std::vector< unsigned char > background_pixels(this->background_width * this->background_height * 4, 0xFF );
    this->background_pixels = background_pixels;

    std::vector< unsigned char > tilemap_pixels(this->tilemap_width * this->tilemap_height * 4, 0xFF );
    this->tilemap_pixels = tilemap_pixels;

    std::vector< unsigned char > spritemap_pixels(this->spritemap_width * this->spritemap_height * 4, 0xFF );
    this->spritemap_pixels = spritemap_pixels;

    this->viewport_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, this->viewport_width, this->viewport_height);
    this->background_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, this->background_width, this->background_height);
    this->spritemap_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,this->spritemap_width, this->spritemap_height);
    this->tilemap_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,this->tilemap_width, this->tilemap_height);

    //  //Initialize SDL_ttf
    if(TTF_Init() == -1)
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

    font = TTF_OpenFont("fonts/VT323-Regular.ttf", 18);
}

static struct timespec frameStart;
struct timespec frameEnd;

void Renderer::render(){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if(debug){
            render_debug();
        }
        SDL_SetRenderTarget(renderer, viewport_texture);
        draw_viewport();

        SDL_RenderCopy(renderer, viewport_texture, NULL, &this->viewport_rect);
        SDL_RenderPresent( renderer );
}

void Renderer::render_debug(){
    // SDL_SetRenderTarget(renderer, debug_texture);
    draw_background();
    SDL_RenderCopy(renderer, background_texture, NULL, &background_rect);

    draw_background_overflow();

    draw_tilemap();
    SDL_RenderCopy(renderer, tilemap_texture, NULL, &tilemap_rect);

    draw_spritemap();
    SDL_RenderCopy(renderer, spritemap_texture, NULL, &spritemap_rect);

    draw_status();
}


void Renderer::draw_text(int x_pos, int y_pos, std::string text){
    SDL_Color textColor = { 0, 0, 0, 0 };
    if(!font) {
        return;
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_FreeSurface(textSurface);
    SDL_Rect renderQuad = { x_pos, y_pos, text_width, text_height };
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

void Renderer::draw_status(){
    draw_text(viewport_width + 30,  0, "A: " + std::to_string(+registers->a));
    draw_text(viewport_width + 100, 0, "F: " + std::to_string(+registers->f));

    draw_text(viewport_width + 30,  20, "B: " + std::to_string(+registers->b));
    draw_text(viewport_width + 100, 20, "C: " + std::to_string(+registers->c));

    draw_text(viewport_width + 30,  40, "H: " + std::to_string(+registers->c));
    draw_text(viewport_width + 100, 40, "L: " + std::to_string(+registers->l));

    draw_text(viewport_width + 50, 60, "SP: " + std::to_string(+registers->sp));
    draw_text(viewport_width + 50, 80, "PC: " + std::to_string(+registers->pc));

    draw_text(viewport_width + 50, 100, "Ticks: " + std::to_string(+memory->clock.t));
}


void Renderer::draw_rectangle(int x, int y, int width, int height, rgb color){
    SDL_Rect rect = {x, y, width, height};
    // rect.x = *ppu->scrollX;
    // rect.y = viewport_height + *ppu->scrollY;
    // rect.w = viewport_width - overflowX;
    // rect.h = viewport_height;
    // rect.x = *ppu->scrollX;
    // rect.y = *ppu->scrollY;
    // rect.w = viewport_width - overflowX;
    // rect.h = viewport_height;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r,color.g,color.b, color.a);
    // SDL_SetRenderDrawColor(renderer, 255,255, 255, 100);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
}
void Renderer::draw_background_overflow(){
    int overflowX = std::max(*ppu->scrollX + viewport_width - background_width, 0);
    int overflowY = std::max(*ppu->scrollY + viewport_height - background_height, 0);

    this->draw_rectangle(*ppu->scrollX,  viewport_height + *ppu->scrollY, viewport_width - overflowX, viewport_height, {255,255,255,100});

    if(overflowX)
        this->draw_rectangle(0,  viewport_height + *ppu->scrollY, overflowX, viewport_height, rgb{0,0,255,100});

    if(overflowY)
        this->draw_rectangle(*ppu->scrollX,  viewport_height, viewport_width - overflowX, overflowY, rgb{0,255,0,100});

    if(overflowX && overflowY)
        this->draw_rectangle(0,  viewport_height, overflowX, overflowY, rgb{255,0,0,100});
}

void Renderer::draw_viewport(){
    for(int i = 0; i < 144 * 160; i++){
        COLOUR color = gpu->framebuffer[i];
        this->viewport_pixels[i * 4 + 0] = color.r;
        this->viewport_pixels[i * 4 + 1] = color.g;
        this->viewport_pixels[i * 4 + 2] = color.b;
        this->viewport_pixels[i * 4 + 3] = color.a;
    }
    SDL_UpdateTexture(viewport_texture, NULL, viewport_pixels.data(), this->viewport_width * 4);
}


void Renderer::draw_tilemap(){
    for(int i = 0; i < 384; i++){
        for(int y = 0; y < 8; y++){
            for(int x = 0; x < 8; x++){
                uint8_t pixel = mmu->tiles[i][y][x];
                int offsetX = ((i * 8 + x) % tilemap_width);
                int offsetY = y + (int(i/16)) * 8;
                int offset = 4 * (offsetY * tilemap_width + offsetX);

                this->tilemap_pixels[ offset + 0 ] = palette[pixel].r;
                this->tilemap_pixels[ offset + 1 ] = palette[pixel].g;
                this->tilemap_pixels[ offset + 2 ] = palette[pixel].b;
                this->tilemap_pixels[ offset + 3 ] = palette[pixel].a;
            }
        }
    }

    SDL_UpdateTexture(this->tilemap_texture, NULL, tilemap_pixels.data(), this->tilemap_width * 4);
}

void Renderer::draw_spritemap(){
    for(int i = 0; i < 40; i++) {
        auto sprite = memory->sprites[i];
        for(int x = 0; x < 8; x++) {
            for(int y = 0; y < 8; y++) {
                uint8_t pixel = mmu->tiles[sprite.tile][y][x];
                int offsetX = ((i * 8 + x) % spritemap_width);
                int offsetY = y +  (int(i/16)) * 8;
                int offset = 4 * (offsetY * tilemap_width + offsetX);
                this->spritemap_pixels[ offset + 0 ] = palette[pixel].r;
                this->spritemap_pixels[ offset + 1 ] = palette[pixel].g;
                this->spritemap_pixels[ offset + 2 ] = palette[pixel].b;
                this->spritemap_pixels[ offset + 3 ] = palette[pixel].a;
            }
        }
    }

    SDL_UpdateTexture(spritemap_texture, NULL, spritemap_pixels.data(), this->spritemap_width * 4);
}

void Renderer::draw_background(){
    int sp = 0;
    for(unsigned short i = 0x9800; i <= 0x9bff; i++) {
        int tile = memory->read_byte(i);
        if(!(memory->read_byte(0xFF40) >> 5 & 0x1) && tile < 128)
            tile += 256 ;
        if ( tile == 0){
            sp++;
            continue;
        }
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                unsigned char color = memory->tiles[tile][y][x];
                int xi = (sp % 32) * 8 + x;
                int yi = (sp / 32) * 8 + y;
                int offset = 4 * ( yi * background_width + xi);
                background_pixels[offset + 0] = palette[color].r;
                background_pixels[offset + 1] = palette[color].g;
                background_pixels[offset + 2] = palette[color].b;
                background_pixels[offset + 3] = palette[color].a;
            }
        }
        sp++;
    }

    for(auto sprite : memory->sprites) {
        for(int x = 0; x < 8; x++) {
            for(int y = 0; y < 8; y++) {

                uint8_t xF = sprite.options.xFlip ? 7 - x : x;
                uint8_t color = memory->tiles[sprite.tile][y][xF];
                if(color){
                    int xi = (memory->read_byte(0xff43) + sprite.x + x) % 256;
                    int yi = memory->read_byte(0xff42) + sprite.y + y;
                    int offset = 4 * ( yi * background_width + xi);
                    background_pixels[offset + 0] = palette[color].r;
                    background_pixels[offset + 1] = palette[color].g;
                    background_pixels[offset + 2] = palette[color].b;
                    background_pixels[offset + 3] = palette[color].a;
                }
            }
        }
    }
    SDL_UpdateTexture(background_texture, NULL, background_pixels.data(), this->background_width * 4);
}

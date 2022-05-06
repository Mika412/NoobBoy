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

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            printf("error initializing SDL: %s\n", SDL_GetError());
    }
     window = SDL_CreateWindow("GAME",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       window_width, window_height, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height );
    frameBuffer = (std::vector<unsigned char>( window_width * window_height * 4, 0 ));
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for( unsigned int i = 0; i < window_width * window_height * 4; i++ )
    {
        frameBuffer[ i ] = 255;
    }

     //Initialize SDL_ttf
    if(TTF_Init() == -1)
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

    font = TTF_OpenFont("fonts/VT323-Regular.ttf", 18);
}

static struct timespec frameStart;
struct timespec frameEnd;

void Renderer::render(){
        long mtime, seconds, useconds;
        clock_gettime(CLOCK_MONOTONIC, &frameEnd);
        seconds = frameEnd.tv_sec - frameStart.tv_sec;
        useconds = frameEnd.tv_nsec - frameStart.tv_nsec;
        mtime = (seconds * 1000 + useconds / (1000.0 * 1000.0));
        if(mtime < 1.0 / 60.0) return;
        clock_gettime(CLOCK_MONOTONIC, &frameStart);

        if(memory->renderFlag.background)
            this->render_background(); 

        if(memory->renderFlag.tiles)
            this->render_tiles(); 

        if(memory->renderFlag.viewport){
            this->render_viewport(); 

            SDL_UpdateTexture(texture, NULL, frameBuffer.data(),window_width * 4);

        }
        SDL_RenderCopy( renderer, texture, NULL, NULL );
        this->render_status();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        int tmpScrollY = 256;
        int overflow = ((tmpScrollY + viewport_height / 2) % background_height) * ((tmpScrollY + viewport_height / 2) / background_height) ;
        // std::cout << "OVERFLOW " << overflow << std::endl;
        SDL_Rect rect;
        rect.x = *gpu->scrollX;
        // rect.y = viewport_height + tmpScrollY;
        rect.y = viewport_height + *gpu->scrollY;
        rect.w = viewport_width;
        rect.h = viewport_height;
        SDL_RenderDrawRect(renderer, &rect);

        // Extra rectangle. When leaves view port
        /* SDL_Rect rect2;
        rect2.x = memory->gpu.scrollX;
        rect2.y = viewport_height + memory->gpu.scrollY;
        rect2.w = viewport_width;
        rect2.h = viewport_height;
        SDL_RenderDrawRect(renderer, &rect2); */

        this->render_status();


        SDL_RenderPresent( renderer );

        memory->renderFlag.viewport = false;
        memory->renderFlag.background = false;
        memory->renderFlag.tiles = false;
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

// std::string Renderer::num_to_hex(int n){
//     std::stringstream stream;
//     stream << std::hex << +registers->a;
//     return stream.str();
// }

void Renderer::render_status(){
    // draw_text(viewport_width + 30,  0, "A: " + num_to_hex(+registers->a));
    // draw_text(viewport_width + 100, 0, "F: " + num_to_hex(+registers->f));

    // draw_text(viewport_width + 30,  20, "B: " + num_to_hex(+registers->b));
    // draw_text(viewport_width + 100, 20, "C: " + num_to_hex(+registers->c));

    // draw_text(viewport_width + 30,  40, "H: " + num_to_hex(+registers->c));
    // draw_text(viewport_width + 100, 40, "L: " + num_to_hex(+registers->l));

    // draw_text(viewport_width + 50, 60, "SP: " + num_to_hex(+registers->sp));
    // draw_text(viewport_width + 50, 80, "PC: " + num_to_hex(+registers->pc));

    // draw_text(viewport_width + 50, 100, "Ticks: " + num_to_hex(cpu->clock.t));

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


void Renderer::render_tiles(){
    int offset = viewport_width + status_width;
    int i, x, y;
    for(i = 0; i < 348; i++) {
        for(x = 0; x < 8; x++) {
            for(y = 0; y < 8; y++) {
                unsigned char color = memory->tiles[i][y][x];
                    int offsetX = offset + ((i * 8 + x) % tiles_width);
                    int offsetY = y +  (int(i/16)) * 8;
                    frameBuffer[4 * ( offsetY * window_width + offsetX) + 0] = palette[color].r;
                    frameBuffer[4 * ( offsetY * window_width + offsetX) + 1] = palette[color].g;
                    frameBuffer[4 * ( offsetY * window_width + offsetX) + 2] = palette[color].b;
                    frameBuffer[4 * ( offsetY * window_width + offsetX) + 3] = palette[color].a;
            }
        }
    }
}

void Renderer::render_viewport(){
    int i, x, y;
    for(i = 0; i < 160 * 144; i++) {
        int x = i % 160;
        int y = i / 160;
        COLOUR color = gpu->framebuffer[i];

        frameBuffer[4 * ( y * window_width + x) + 0] = color.r;
        frameBuffer[4 * ( y * window_width + x) + 1] = color.g;
        frameBuffer[4 * ( y * window_width + x) + 2] = color.b;
        frameBuffer[4 * ( y * window_width + x) + 3] = color.a;
    }
}

void Renderer::render_background(){
    int sp = 0;
    for(unsigned short i = 0x9800; i <= 0x9bff; i++) {
        unsigned char tile = memory->read_byte(i);
        if ( tile == 0){
            sp++;
            continue;
        }
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                unsigned char color = memory->tiles[tile][y][x];

                int xi = (sp % 32) * 8 + x;
                int yi = viewport_height + (sp / 32) * 8 + y;
                frameBuffer[4 * ( yi * window_width + xi) + 0] = palette[color].r;
                frameBuffer[4 * ( yi * window_width + xi) + 1] = palette[color].g;
                frameBuffer[4 * ( yi * window_width + xi) + 2] = palette[color].b;
                frameBuffer[4 * ( yi * window_width + xi) + 3] = palette[color].a;
            }
        }
        sp++;
    }
}

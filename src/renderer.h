#include "mmu.h"
#include "ppu.h"
#include "cpu.h"
#include "status.h"
#include "interrupt.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iomanip>

class Renderer{
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;

        SDL_Texture *viewport_texture;
        SDL_Texture *debug_texture;
        SDL_Texture *background_texture;
        SDL_Texture *tilemap_texture;
        SDL_Texture *spritemap_texture;

        CPU *cpu;
        PPU *ppu;
        Registers *registers;
        Interrupts *interrupts;
        MMU *mmu;
        Status *status;


        int status_width = 160;
        int status_height = 144;

        // Viewport 
        int viewport_width = 160;
        int viewport_height = 144;
        std::vector<uint8_t> viewport_pixels;
        SDL_Rect viewport_rect = {0,0,viewport_width,viewport_height};

        // Tilemap
        int tilemap_width = 128;
        int tilemap_height = 256;
        std::vector<uint8_t> tilemap_pixels;
        SDL_Rect tilemap_rect = {0, viewport_height, tilemap_width, tilemap_height};

        // Spritemap
        int spritemap_height = 64;
        int spritemap_width = 40;
        std::vector<uint8_t> spritemap_pixels;
        SDL_Rect spritemap_rect = {tilemap_width, viewport_height, spritemap_width * 2, spritemap_height * 2};

        // VRAM  
        int background_width = 256;
        int background_height = 256;
        std::vector<uint8_t> background_pixels;
        SDL_Rect background_rect = {tilemap_width + spritemap_width * 2, viewport_height, background_width, background_height};

        int debug_texture_height = viewport_height + background_height;
        int debug_texture_width = background_width + tilemap_width + spritemap_width * 2;

        int window_height = viewport_height;
        int window_width = viewport_width;
        TTF_Font *font;
    

        int framerate_time = 1000 / 60;
        std::chrono::steady_clock::time_point startFrame;
        std::chrono::steady_clock::time_point endFrame;
        
        void draw_viewport();
        void draw_background();
        void draw_tilemap();
        void draw_spritemap();
        void draw_status();
        void draw_background_overflow();
        void draw_rectangle(int x, int y, int width, int height, Colour color);

        // Helper functions
        void draw_text(int x_pop, int y_pos, std::string text);
        void render_debug();
        void check_framerate();

    public:
        Renderer(Status *status, CPU *cpu, PPU *ppu, Registers *registers, Interrupts *interrupts, MMU *mmu);
        void render();
};

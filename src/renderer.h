#include "mmu.h"
#include "ppu.h"
#include "cpu.h"
#include "status.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <chrono>
#include <thread>

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
        MMU *memory;
        MMU *mmu;
        Status *status;


        int status_width = 160;
        int status_height = 144;

        int tiles_width = 128;
        int tiles_height = 144;

        int sprites_width = 128;
        int sprites_height = 144;

        // Viewport 
        int viewport_width = 160;
        int viewport_height = 144;
        std::vector<unsigned char> viewport_pixels;
        SDL_Rect viewport_rect = {0,0,viewport_width,viewport_height};

        // VRAM  
        int background_width = 256;
        int background_height = 256;
        std::vector<unsigned char> background_pixels;
        SDL_Rect background_rect = {0, viewport_height, background_width, background_height};

        // Tilemap
        int tilemap_width = 128;
        int tilemap_height = 256;
        std::vector<unsigned char> tilemap_pixels;
        SDL_Rect tilemap_rect = {background_width, viewport_height, tilemap_width, tilemap_height};

        // Spritemap
        int spritemap_width = 128;
        int spritemap_height = 256;
        std::vector<unsigned char> spritemap_pixels;
        SDL_Rect spritemap_rect = {background_width + tilemap_width, 0, sprites_width, sprites_height};

        int debug_texture_height = viewport_height + background_height;
        int debug_texture_width = background_width + tilemap_width + spritemap_width;

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
        void draw_rectangle(int x, int y, int width, int height, rgb color);

        // Helper functions
        void draw_text(int x_pop, int y_pos, std::string text);
        void render_debug();
        void check_framerate();

    public:
        Renderer(Status *status, CPU *cpu, PPU *ppu, Registers *registers, MMU *memory);
        void render();
};

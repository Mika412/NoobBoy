#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdlib.h>
#include <unistd.h>

#include <array>
#include <bitset>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <algorithm>

#include "cpu.h"
#include "interrupt.h"
#include "mmu.h"
#include "ppu.h"
#include "status.h"

class Renderer {
   public:
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Texture *viewport_texture;

    CPU *cpu;
    PPU *ppu;
    Registers *registers;
    Interrupts *interrupts;
    MMU *mmu;
    Status *status;

    // Viewport
    int viewport_width = 160;
    int viewport_height = 144;
    std::array<uint8_t, 160 * 144 * 4> viewport_pixels;
    SDL_FRect viewport_rect = {0, 0, (float)viewport_width, (float)viewport_height};
    float window_height = viewport_height;
    float window_width = viewport_width;

    int framerate_time = 1000 / 60;
    std::chrono::steady_clock::time_point startFrame;
    std::chrono::steady_clock::time_point endFrame;
    void check_framerate();

    void init_window(float window_width, float window_height);
    void draw_viewport();

   public:
    Renderer(Status *status, CPU *cpu, PPU *ppu, Registers *registers, Interrupts *interrupts, MMU *mmu);
    void render();
    virtual void init();
    virtual void draw();
};

class DebugRenderer : public Renderer {
   private:
    SDL_Texture *debug_texture;
    SDL_Texture *background_texture;
    SDL_Texture *tilemap_texture;
    SDL_Texture *spritemap_texture;

    TTF_Font *font;

    // Tilemap
    int tilemap_width = 128;
    int tilemap_height = 256;
    std::array<uint8_t, 128 * 256 * 4> tilemap_pixels;
    SDL_FRect tilemap_rect = {0, (float)viewport_height, (float)tilemap_width, (float)tilemap_height};

    // Spritemap
    int spritemap_height = 64;
    int spritemap_width = 40;
    std::array<uint8_t, 64 * 40 * 4> spritemap_pixels;
    SDL_FRect spritemap_rect = {(float)tilemap_width, (float)viewport_height, (float)spritemap_width * 2,
                                (float)spritemap_height * 2};

    // VRAM
    int background_width = 256;
    int background_height = 256;
    std::array<uint8_t, 256 * 256 * 4> background_pixels;
    SDL_FRect background_rect = {(float)(tilemap_width + spritemap_width * 2), (float)viewport_height,
                                 (float)background_width, (float)background_height};

    int window_height = viewport_height + background_height;
    int window_width = background_width + tilemap_width + spritemap_width * 2;

    void draw() override;
    void draw_background();
    void draw_tilemap();
    void draw_spritemap();
    void draw_status();
    void draw_background_overflow();

    // Helper functions
    void draw_text(int x_pop, int y_pos, std::string text);
    void draw_rectangle(int x, int y, int width, int height, Colour color);

   public:
    using Renderer::Renderer;

    void init() override;
};

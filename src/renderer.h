#include "mmu.h"
#include "gpu.h"
#include "cpu.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <vector>

class Renderer{
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;

        std::vector<unsigned char> frameBuffer;

        CPU *cpu;
        GPU *gpu;
        Registers *registers;
        MMU *memory;

        int viewport_width = 160;
        int viewport_height = 144;

        int status_width = 160;
        int status_height = 144;

        int tiles_width = 128;
        int tiles_height = 144;

        int background_width = 256;
        int background_height = 256;

        int window_height = viewport_height + background_height;
        int window_width = viewport_width + status_width + tiles_width;

        TTF_Font *font;

        void render_tiles();
        void render_viewport();
        void render_status();
        void render_background();

        // Helper functions
        std::string num_to_hex(int n);
        void draw_text(int x_pop, int y_pos, std::string text);

    public:
        Renderer(CPU *cpu, GPU *gpu, Registers *registers, MMU *memory);
        void render();
};

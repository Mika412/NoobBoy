#include "renderer.h"

Renderer::Renderer(Status *status, CPU *cpu, PPU *gpu, Registers *registers, Interrupts *interrupts, MMU *mmu) {
    this->cpu = cpu;
    this->ppu = gpu;
    this->registers = registers;
    this->mmu = mmu;
    this->interrupts = interrupts;
    this->status = status;
}
void Renderer::init() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(this->window_width * 2, window_height * 2, 0, &this->window, &this->renderer);
    SDL_RenderSetLogicalSize(this->renderer, this->window_width, this->window_height);
    SDL_SetWindowResizable(this->window, SDL_TRUE);
    SDL_SetWindowTitle(this->window, mmu->cartridge->rom_title);

    std::vector<uint8_t> viewport_pixels(this->viewport_width * this->viewport_height * 4, 0xFF);
    this->viewport_pixels = viewport_pixels;

    this->viewport_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, this->viewport_width, this->viewport_height);

    //  //Initialize SDL_ttf
    if (TTF_Init() == -1)
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

    font = TTF_OpenFont("fonts/VT323-Regular.ttf", 18);
}

void Renderer::render() {
    this->check_framerate();

    switch (this->status->colorMode) {
        case NORMAL:
            SDL_SetTextureColorMod(viewport_texture, 255, 255, 255);
            break;
        case RETRO:
            SDL_SetTextureColorMod(viewport_texture, 155, 188, 15);
            break;
        case GRAY:
            SDL_SetTextureColorMod(viewport_texture, 224, 219, 205);
            break;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    draw();

    SDL_RenderCopy(renderer, viewport_texture, NULL, &this->viewport_rect);
    SDL_RenderPresent(renderer);
}

void Renderer::check_framerate() {
    endFrame = std::chrono::steady_clock::now();
    auto timeTook = std::chrono::duration_cast<std::chrono::milliseconds>(endFrame - startFrame).count();
    if (timeTook < framerate_time)
        std::this_thread::sleep_for(std::chrono::milliseconds(framerate_time - timeTook));

    startFrame = std::chrono::steady_clock::now();
}

void Renderer::draw() {
    draw_viewport();
}

void Renderer::draw_viewport() {
    for (int i = 0; i < 144 * 160; i++) {
        Colour colour = ppu->framebuffer[i];
        std::copy(colour.colours, colour.colours + 4, viewport_pixels.begin() + i * 4);
    }
    SDL_UpdateTexture(viewport_texture, NULL, viewport_pixels.data(), this->viewport_width * 4);
}

void DebugRenderer::draw_spritemap() {
    auto draw_sprite = [this](MMU::Sprite sprite, int tile_off, int off_x, int off_y) {
        if (!sprite.ready)
            return;
        for (int x = 0; x < 8; x++) {
            uint8_t xF = sprite.options.xFlip ? 7 - x : x;
            for (int y = 0; y < 8; y++) {
                uint8_t yF = sprite.options.yFlip ? 7 - y : y;
                uint8_t colour_n = mmu->tiles[sprite.tile + tile_off].pixels[yF][xF];
                int offsetX = ((off_x + x) % spritemap_width);
                int offsetY = y + off_y;
                int offset = 4 * (offsetY * spritemap_width + offsetX);

                Colour colour = sprite.colourPalette[colour_n];
                std::copy(colour.colours, colour.colours + 4, spritemap_pixels.begin() + offset);
            }
        }
    };

    int row = 0;
    if (ppu->control->spriteSize) {
        for (int i = 0, row = 0; i < 20; i++) {
            draw_sprite(mmu->sprites[i], 0, i * 8, row * 8);
            draw_sprite(mmu->sprites[i], 1, i * 8, row * 8 + 8);
            if (((i + 1) % 5) == 0)
                row += 2;
        }
    } else {
        for (int i = 0, row = 0; i < 40; i++) {
            draw_sprite(mmu->sprites[i], 0, i * 8, row * 8);
            if (((i + 1) % 5) == 0)
                row++;
        }
    }

    SDL_UpdateTexture(spritemap_texture, NULL, spritemap_pixels.data(), this->spritemap_width * 4);
}

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
    viewport_pixels.fill(0xFF);

    this->init_window(this->window_width, this->window_height);

    this->viewport_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                               this->viewport_width, this->viewport_height);
}

void Renderer::init_window(int window_width, int window_height) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(window_width * 2, window_height * 2, 0, &this->window, &this->renderer);
    SDL_RenderSetLogicalSize(this->renderer, window_width, window_height);
    SDL_SetWindowResizable(this->window, SDL_TRUE);
    SDL_SetWindowTitle(this->window, mmu->cartridge->rom_title.c_str());
}

void Renderer::check_framerate() {
    endFrame = std::chrono::steady_clock::now();
    auto timeTook = std::chrono::duration_cast<std::chrono::milliseconds>(endFrame - startFrame).count();
    if (timeTook < framerate_time)
        std::this_thread::sleep_for(std::chrono::milliseconds(framerate_time - timeTook));

    startFrame = std::chrono::steady_clock::now();
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
    SDL_SetRenderTarget(renderer, viewport_texture);

    draw();

    SDL_RenderCopy(renderer, viewport_texture, NULL, &this->viewport_rect);
    SDL_RenderPresent(renderer);
}

void Renderer::draw() { draw_viewport(); }

void Renderer::draw_viewport() {
    for (int i = 0; i < 144 * 160; i++) {
        Colour colour = ppu->framebuffer[i];
        std::copy(colour.colours, colour.colours + 4, viewport_pixels.begin() + i * 4);
    }
    SDL_UpdateTexture(viewport_texture, NULL, viewport_pixels.data(), this->viewport_width * 4);
}

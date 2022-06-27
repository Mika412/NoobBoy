#include "renderer.h"

Renderer::Renderer(Status *status, CPU *cpu, PPU *gpu, Registers *registers, Interrupts *interrupts, MMU *mmu){
    this->cpu = cpu;
    this->ppu = gpu;
    this->registers = registers;
    this->mmu = mmu;
    this->interrupts = interrupts;
    this->status = status;

    SDL_Init(SDL_INIT_VIDEO);
    if(status->debug){
        window_width = debug_texture_width;
        window_height = debug_texture_height;
    }
    SDL_CreateWindowAndRenderer(this->window_width*2, this->window_height*2, 0, &this->window, &this->renderer);
    SDL_RenderSetLogicalSize(this->renderer, this->window_width, this->window_height);
    SDL_SetWindowResizable(this->window, SDL_TRUE);
    //
    std::vector<uint8_t> viewport_pixels(this->viewport_width * this->viewport_height * 4, 0xFF );
    this->viewport_pixels = viewport_pixels;

    std::vector<uint8_t> background_pixels(this->background_width * this->background_height * 4, 0xFF );
    this->background_pixels = background_pixels;

    std::vector<uint8_t> tilemap_pixels(this->tilemap_width * this->tilemap_height * 4, 0xFF );
    this->tilemap_pixels = tilemap_pixels;

    std::vector<uint8_t> spritemap_pixels(this->spritemap_width * this->spritemap_height * 4, 0xFF );
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

std::string to_hex_string( const uint i ) {
    std::stringstream s;
    s << "0x" <<  std::setfill('0') << std::setw(2) << std::uppercase << std::hex << i;
    return s.str();
}

void Renderer::render(){
    this->check_framerate();

    switch(this->status->colorMode){
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

    if(status->debug)
        render_debug();

    SDL_SetRenderTarget(renderer, viewport_texture);
    draw_viewport();

    SDL_RenderCopy(renderer, viewport_texture, NULL, &this->viewport_rect);
    SDL_RenderPresent( renderer );
}

void Renderer::check_framerate() {
    endFrame = std::chrono::steady_clock::now();
    auto timeTook = std::chrono::duration_cast<std::chrono::milliseconds>(endFrame - startFrame).count();
    if(timeTook < framerate_time)
        std::this_thread::sleep_for(std::chrono::milliseconds(framerate_time - timeTook));

    startFrame = std::chrono::steady_clock::now();
}

void Renderer::render_debug(){
    draw_background();
    draw_tilemap();
    draw_spritemap();

    SDL_RenderCopy(renderer, background_texture, NULL, &background_rect);
    SDL_RenderCopy(renderer, tilemap_texture, NULL, &tilemap_rect);
    SDL_RenderCopy(renderer, spritemap_texture, NULL, &spritemap_rect);

    draw_background_overflow();
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
    draw_text(viewport_width + 10,  0, "A: " + std::to_string(+registers->a));
    draw_text(viewport_width + 80, 0, "F: " + std::to_string(+registers->f));

    draw_text(viewport_width + 10,  20, "B: " + std::to_string(+registers->b));
    draw_text(viewport_width + 80, 20, "C: " + std::to_string(+registers->c));

    draw_text(viewport_width + 30,  40, "H: " + std::to_string(+registers->c));
    draw_text(viewport_width + 80, 40, "L: " + std::to_string(+registers->l));

    draw_text(viewport_width + 30, 60, "SP: " + std::to_string(+registers->sp));
    draw_text(viewport_width + 30, 80, "PC: " + std::to_string(+registers->pc));

    draw_text(viewport_width + 10, 100, "Ticks: " + std::to_string(+mmu->clock.t));

    draw_text(viewport_width + 10, 120, "IME: " + std::to_string(+interrupts->is_master_enabled()));
    draw_text(viewport_width + 80, 120, "HALT: " + std::to_string(+mmu->is_halted));


    draw_text(viewport_width + 150, 0,      "IE:   " + to_hex_string(mmu->read_byte(0xFFFF)) + " (" + std::bitset<8>(mmu->read_byte(0xFFFF)).to_string() + ")");
    draw_text(viewport_width + 150, 20,     "IF:   " + to_hex_string(mmu->read_byte(0xFF0F)) + " (" + std::bitset<8>(mmu->read_byte(0xFF0F)).to_string() + ")");
    draw_text(viewport_width + 150, 40,     "LCDC: " + to_hex_string(mmu->read_byte(0xFF40)) + " (" + std::bitset<8>(mmu->read_byte(0xFF40)).to_string() + ")");
    draw_text(viewport_width + 150, 60,     "DIV:  " + to_hex_string(mmu->read_byte(0xFF04)) + " (" + std::bitset<8>(mmu->read_byte(0xFF04)).to_string() + ")");
    draw_text(viewport_width + 150, 80,     "TIMA: " + to_hex_string(mmu->read_byte(0xFF05)) + " (" + std::bitset<8>(mmu->read_byte(0xFF05)).to_string() + ")");
    draw_text(viewport_width + 150, 100,    "TMA:  " + to_hex_string(mmu->read_byte(0xFF06)) + " (" + std::bitset<8>(mmu->read_byte(0xFF06)).to_string() + ")");
    draw_text(viewport_width + 150, 120,    "TAC:  " + to_hex_string(mmu->read_byte(0xFF07)) + " (" + std::bitset<8>(mmu->read_byte(0xFF07)).to_string() + ")");
    
    // Draw paused message
    if(this->status->isPaused){
        this->draw_rectangle(window_width - 58,  window_height - 22, 55, 20, {255,0,0,100});
        draw_text(window_width - 55, window_height - 22, "STOPPED");
    }
}


void Renderer::draw_rectangle(int x, int y, int width, int height, Colour color){
    SDL_Rect rect = {x, y, width, height};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r,color.g,color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void Renderer::draw_background_overflow(){
    int overflowX = std::max(*ppu->scrollX + viewport_width - background_width, 0);
    int overflowY = std::max(*ppu->scrollY + viewport_height - background_height, 0);

    this->draw_rectangle(background_rect.x + *ppu->scrollX,  viewport_height + *ppu->scrollY, viewport_width - overflowX, viewport_height, {255,255,255,100});

    if(overflowX)
        this->draw_rectangle(background_rect.x,  viewport_height + *ppu->scrollY, overflowX, viewport_height, {255,255,255,100});

    if(overflowY)
        this->draw_rectangle(background_rect.x + *ppu->scrollX,  viewport_height, viewport_width - overflowX, overflowY, {255,255,255,100});

    if(overflowX && overflowY)
        this->draw_rectangle(background_rect.x,  viewport_height, overflowX, overflowY, {255,255,255,100});
}

void Renderer::draw_viewport(){
    for(int i = 0; i < 144 * 160; i++){
        Colour colour = ppu->framebuffer[i];
        std::copy(colour.colours, colour.colours + 4, viewport_pixels.begin() + i * 4);
    }
    SDL_UpdateTexture(viewport_texture, NULL, viewport_pixels.data(), this->viewport_width * 4);
}


void Renderer::draw_tilemap(){
    for(int i = 0; i < 384; i++){
        for(int y = 0; y < 8; y++){
            for(int x = 0; x < 8; x++){
                uint8_t colour_n = mmu->tiles[i].pixels[y][x];
                int offsetX = ((i * 8 + x) % tilemap_width);
                int offsetY = y + (int(i/16)) * 8;
                int offset = 4 * (offsetY * tilemap_width + offsetX);

                Colour colour = mmu->palette_BGP[colour_n];
                std::copy(colour.colours, colour.colours + 4, tilemap_pixels.begin() + offset);
            }
        }
    }

    SDL_UpdateTexture(this->tilemap_texture, NULL, tilemap_pixels.data(), this->tilemap_width * 4);
}

void Renderer::draw_spritemap(){
    auto draw_sprite = [this](MMU::Sprite sprite, int tile_off, int off_x, int off_y) {
        if(!sprite.ready) return;
        for(int x = 0; x < 8; x++) {
            uint8_t xF = sprite.options.xFlip ? 7 - x : x;
            for(int y = 0; y < 8; y++) {
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
    if(ppu->control->spriteSize){
        for(int i = 0, row = 0; i < 20; i++) {
            draw_sprite(mmu->sprites[i], 0, i * 8, row * 8);
            draw_sprite(mmu->sprites[i], 1, i * 8, row * 8 + 8);
            if(((i + 1) % 5) == 0) row+=2;
        }
    } else {
        for(int i = 0, row = 0; i < 40; i++) {
            draw_sprite(mmu->sprites[i], 0, i * 8, row * 8);
            if(((i + 1) % 5) == 0) row++;
        }
    }

    SDL_UpdateTexture(spritemap_texture, NULL, spritemap_pixels.data(), this->spritemap_width * 4);
}

void Renderer::draw_background(){
    for(int i = 0; i <= 1023; i++) {
        int tile = mmu->read_byte(0x9800 + i);
        if(!this->ppu->control->bgWindowDataSelect && tile < 128)
            tile += 256;
        
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                uint8_t color = mmu->tiles[tile].pixels[y][x];
                int xi = (i % 32) * 8 + x;
                int yi = (i / 32) * 8 + y;
                int offset = 4 * ( yi * background_width + xi);
                Colour colour = mmu->palette_BGP[color];
                std::copy(colour.colours, colour.colours + 4, background_pixels.begin() + offset);
            }
        }
    }

    // Draw sprites
    for(auto sprite : mmu->sprites) {
        if(!sprite.ready) continue;
        for(int tile_num = 0; tile_num < 1 + int(ppu->control->spriteSize); tile_num++){
            int y_pos = sprite.y + tile_num * 8;
            // Iterate over both tiles
            for(int x = 0; x < 8; x++) {
                for(int y = 0; y < 8; y++) {
                    uint8_t xF = sprite.options.xFlip ? 7 - x : x;
                    uint8_t yF = sprite.options.yFlip ? 7 - y : y;

                    int tile = sprite.tile & (ppu->control->spriteSize ? 0xFE : 0xFF);
                    uint8_t colour_n = mmu->tiles[tile + tile_num].pixels[yF][xF];

                    if(!colour_n)
                        continue;
                    int xi = (mmu->read_byte(0xff43) + sprite.x + x) % 256;
                    int yi = (mmu->read_byte(0xff42) + y_pos + y) % 256;
                    int offset = 4 * ( yi * background_width + xi);

                    if(offset >= background_pixels.size())
                        continue;

                    Colour colour = sprite.colourPalette[colour_n];
                    std::copy(colour.colours, colour.colours + 4, background_pixels.begin() + offset);
                }
            }
        }
    }
    SDL_UpdateTexture(background_texture, NULL, background_pixels.data(), this->background_width * 4);
}

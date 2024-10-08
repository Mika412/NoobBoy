#include "ppu.h"

PPU::PPU(Registers* registers, Interrupts* interrupts, MMU* mmu) {
    this->registers = registers;
    this->mmu = mmu;
    this->interrupts = interrupts;

    control = (Control*)&mmu->memory[0xff40];
    stat = (Stat*)&mmu->memory[0xff41];
    scrollY = &mmu->memory[0xff42];
    scrollX = &mmu->memory[0xff43];
    scanline = &mmu->memory[0xff44];
}

void PPU::step() {
    modeclock += mmu->clock.t_instr;

    if (!control->lcdEnable) {
        mode = 0;
        if (modeclock >= 70224)
            modeclock -= 70224;
        return;
    }

    switch (mode) {
        case 0:  // HBLANK
            if (modeclock >= 204) {
                modeclock -= 204;
                mode = 2;

                *scanline += 1;
                compare_ly_lyc();

                if (*scanline == 144) {
                    mode = 1;
                    can_render = true;
                    interrupts->set_interrupt_flag(INTERRUPT_VBLANK);
                    if (stat->vblank_interrupt)
                        interrupts->set_interrupt_flag(INTERRUPT_LCD);
                } else if (stat->oam_interrupt)
                    interrupts->set_interrupt_flag(INTERRUPT_LCD);

                mmu->write_byte(0xff41, (mmu->read_byte(0xff41) & 0xFC) | (mode & 3));
            }
            break;
        case 1:  // VBLANK
            if (modeclock >= 456) {
                modeclock -= 456;
                *scanline += 1;
                compare_ly_lyc();
                if (*scanline == 153) {
                    *scanline = 0;
                    mode = 2;
                    mmu->write_byte(0xff41, (mmu->read_byte(0xff41) & 0xFC) | (mode & 3));
                    if (stat->oam_interrupt)
                        interrupts->set_interrupt_flag(INTERRUPT_LCD);
                }
            }

            break;
        case 2:  // OAM
            if (modeclock >= 80) {
                modeclock -= 80;
                mode = 3;
                mmu->write_byte(0xff41, (mmu->read_byte(0xff41) & 0xFC) | (mode & 3));
            }
            break;
        case 3:  // VRAM
            if (modeclock >= 172) {
                modeclock -= 172;
                mode = 0;
                render_scan_lines();
                mmu->write_byte(0xff41, (mmu->read_byte(0xff41) & 0xFC) | (mode & 3));

                if (stat->hblank_interrupt)
                    interrupts->set_interrupt_flag(INTERRUPT_LCD);
            }
            break;
    }
}

void PPU::compare_ly_lyc() {
    uint8_t lyc = mmu->read_byte(0xFF45);
    stat->coincidence_flag = int(lyc == *scanline);

    if (lyc == *scanline && stat->coincidence_interrupt)
        this->interrupts->set_interrupt_flag(INTERRUPT_LCD);
}

void PPU::render_scan_lines() {
    bool row_pixels[160] = {0};
    this->render_scan_line_background(row_pixels);
    this->render_scan_line_window();
    this->render_scan_line_sprites(row_pixels);
}

void PPU::render_scan_line_background(bool* row_pixels) {
    uint16_t address = 0x9800;

    if (this->control->bgDisplaySelect)
        address += 0x400;

    address += ((*this->scrollY + *this->scanline) / 8 * 32) % (32 * 32);

    uint16_t start_row_address = address;
    uint16_t end_row_address = address + 32;
    address += (*this->scrollX >> 3);

    int x = *this->scrollX & 7;
    int y = (*this->scanline + *this->scrollY) & 7;
    int pixelOffset = *this->scanline * 160;

    int pixel = 0;
    for (int i = 0; i < 21; i++) {
        uint16_t tile_address = address + i;
        if (tile_address >= end_row_address)
            tile_address = (start_row_address + tile_address % end_row_address);

        int tile = this->mmu->read_byte(tile_address);
        if (!this->control->bgWindowDataSelect && tile < 128)
            tile += 256;

        for (; x < 8; x++) {
            if (pixel >= 160)
                break;

            int colour = mmu->tiles[tile].pixels[y][x];
            framebuffer[pixelOffset++] = mmu->palette_BGP[colour];
            if (colour > 0)
                row_pixels[pixel] = true;
            pixel++;
        }
        x = 0;
    }
}

void PPU::render_scan_line_window() {
    if (!this->control->windowEnable) {
        return;
    }

    if (mmu->read_byte(0xFF4A) > *this->scanline) {
        return;
    }

    uint16_t address = 0x9800;
    if (this->control->windowDisplaySelect)
        address += 0x400;

    address += ((*this->scanline - mmu->read_byte(0xFF4A)) / 8) * 32;
    int y = (*this->scanline - mmu->read_byte(0xFF4A)) & 7;
    int x = 0;

    int pixelOffset = *this->scanline * 160;
    pixelOffset += mmu->read_byte(0xFF4B) - 7;
    for (uint16_t tile_address = address; tile_address < address + 20; tile_address++) {
        int tile = this->mmu->read_byte(tile_address);

        if (!this->control->bgWindowDataSelect && tile < 128)
            tile += 256;

        for (; x < 8; x++) {
            if (pixelOffset > sizeof(framebuffer))
                continue;
            int colour = mmu->tiles[tile].pixels[y][x];
            framebuffer[pixelOffset++] = mmu->palette_BGP[colour];
        }
        x = 0;
    }
}

void PPU::render_scan_line_sprites(bool* row_pixels) {
    int sprite_height = control->spriteSize ? 16 : 8;

    bool visible_sprites[40];
    int sprite_row_count = 0;

    for (int i = 39; i >= 0; i--) {
        auto sprite = mmu->sprites[i];

        if (!sprite.ready) {
            visible_sprites[i] = false;
            continue;
        }

        if ((sprite.y > *scanline) || ((sprite.y + sprite_height) <= *scanline)) {
            visible_sprites[i] = false;
            continue;
        }

        visible_sprites[i] = sprite_row_count++ <= 10;
    }

    for (int i = 39; i >= 0; i--) {
        if (!visible_sprites[i])
            continue;

        auto sprite = mmu->sprites[i];

        if ((sprite.x < -7) || (sprite.x >= 160))
            continue;

        // Flip vertically
        int pixel_y = *scanline - sprite.y;
        pixel_y = sprite.options.yFlip ? (7 + 8 * control->spriteSize) - pixel_y : pixel_y;

        for (int x = 0; x < 8; x++) {
            int tile_num = sprite.tile & (control->spriteSize ? 0xFE : 0xFF);
            int colour = 0;

            int x_temp = sprite.x + x;
            if (x_temp < 0 || x_temp >= 160)
                continue;

            int pixelOffset = *this->scanline * 160 + x_temp;

            // Flip horizontally
            uint8_t pixel_x = sprite.options.xFlip ? 7 - x : x;

            if (control->spriteSize && (pixel_y >= 8))
                colour = mmu->tiles[tile_num + 1].pixels[pixel_y - 8][pixel_x];
            else
                colour = mmu->tiles[tile_num].pixels[pixel_y][pixel_x];

            // Black is transparent
            if (!colour)
                continue;

            if (!row_pixels[x_temp] || !sprite.options.renderPriority)
                framebuffer[pixelOffset] = sprite.colourPalette[colour];
        }
    }
}

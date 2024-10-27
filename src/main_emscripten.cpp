#include "gb.h"
#include <emscripten.h>

#include <emscripten/bind.h>

GB gameboy;
void run_until_next_frame() {
    if (!gameboy.status.isRunning)
        return;
    gameboy.run_until_next_frame();
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void loadGame() {
    std::cout << "Loading game" << std::endl;
    std::string rom = "/game.gb";
    std::string bootrom = "";
    std::string save_file = "";
    int debug_flag = 0;
    int sound_flag = 0;
    int no_bootrom = 0;

    gameboy.init(rom, no_bootrom, bootrom, save_file, debug_flag, sound_flag);
    gameboy.status.isPaused = true;
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void toggleGame() { gameboy.status.isPaused = !gameboy.status.isPaused; }
}
extern "C" {
EMSCRIPTEN_KEEPALIVE
void setColorMode(int mode) { gameboy.status.colorMode = mode; }
}

std::vector<std::string> getROMInfo() {
    std::string is_cgb_game = (gameboy.cartridge->cgb_game ? "Yes" : "No");
    return {gameboy.cartridge->rom_title, is_cgb_game, std::to_string(gameboy.cartridge->mbc_type),
            std::to_string(gameboy.cartridge->rom_banks_count), std::to_string(gameboy.cartridge->ram_banks_count)};
}

EMSCRIPTEN_BINDINGS(module) {
    emscripten::function("getROMInfo", &getROMInfo);
    emscripten::register_vector<std::string>("vector<string>");
}

int main(int argc, char *argv[]) {
    emscripten_set_main_loop(run_until_next_frame, 0, 0);
    return 0;
}

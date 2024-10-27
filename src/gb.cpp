#include "gb.h"

void GB::init(std::string rom, bool no_bootrom, std::string bootrom, std::string save_file, bool debug, bool sound) {
    Cartridge *cartridge = new Cartridge(rom, save_file);
    this->init(cartridge, no_bootrom, bootrom, debug, sound);
}

void GB::init(Cartridge *cartridge, bool no_bootrom, std::string bootrom, bool debug, bool sound) {
    this->cartridge = cartridge;
    mmu = new MMU(cartridge);

    interrupts = new Interrupts(&registers, mmu);
    cpu = new CPU(&registers, interrupts, mmu);
    ppu = new PPU(&registers, interrupts, mmu);
    timer = new Timer(mmu, interrupts);
    joypad = new Joypad(&status, interrupts, mmu);
    status.debug = debug;

    if (no_bootrom)
        cpu->no_bootrom_init();
    else if (!bootrom.empty())
        mmu->load_boot_rom(bootrom);
    else
        mmu->load_default_boot_rom();

    if (sound)
        apu = new APU(&status, mmu);

    if (debug)
        renderer = new DebugRenderer(&status, cpu, ppu, &registers, interrupts, mmu);
    else
        renderer = new Renderer(&status, cpu, ppu, &registers, interrupts, mmu);
    renderer->init();

    status.isRunning = true;
}

bool GB::run_step() {
    if (!status.isPaused || status.doStep) {
        mmu->clock.t_instr = 0;
        bool interrupted = interrupts->check();
        if (!interrupted)
            cpu->step();

        timer->inc();
        ppu->step();
    }

    status.doStep = false;
    joypad->check(mmu->clock.t_instr);

    if (ppu->can_render || status.isPaused) {
        renderer->render();
        ppu->can_render = false;
        return true;
    }
    return false;
}

void GB::run() {
    while (status.isRunning) {
        this->run_step();
    }
}

void GB::run_until_next_frame() {
    while (!this->run_step()) {
    }
}

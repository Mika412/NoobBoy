#include "gb.h"

void GB::init(std::string rom, std::string bootrom, std::string save_file, bool debug) {
    Cartridge *cartridge = new Cartridge(rom, save_file);
    this->init(cartridge, bootrom, debug);
}

void GB::init(Cartridge *cartridge, std::string bootrom, bool debug) {
    mmu = new MMU(cartridge);

    interrupts = new Interrupts(&registers, mmu);
    cpu = new CPU(&registers, interrupts, mmu);
    ppu = new PPU(&registers, interrupts, mmu);
    apu = new APU(mmu);
    timer = new Timer(mmu, interrupts);
    joypad = new Joypad(&status, interrupts, mmu);
    status.debug = debug;

    if (!bootrom.empty())
        mmu->load_boot_rom(bootrom);
    else
        cpu->no_bootrom_init();

    if (debug)
        renderer = new DebugRenderer(&status, cpu, ppu, &registers, interrupts, mmu);
    else
        renderer = new Renderer(&status, cpu, ppu, &registers, interrupts, mmu);
    renderer->init();
}

void GB::run() {
    while (status.isRunning) {
        if (!status.isPaused || status.doStep) {
            mmu->clock.t_instr = 0;
            bool interrupted = interrupts->check();
            if (!interrupted)
                cpu->step();

            timer->inc();
            ppu->step();
        }

        if (ppu->can_render || status.isPaused) {
            renderer->render();
            ppu->can_render = false;
        }

        status.doStep = false;
        joypad->check(mmu->clock.t_instr);
    }
}

int main(int argc, char *argv[]) {
    std::string rom = "";
    std::string bootrom = "";
    std::string save_file = "";
    int debug_flag = 0;

    static struct option long_options[] = {
        {"debug", no_argument, &debug_flag, true},
        {"bootrom", required_argument, 0, 'b'},
        {"rom", required_argument, 0, 'r'},
        {"load-save", required_argument, 0, 'l'},
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "b:r:l:", long_options, &option_index)) != EOF) {
        if (-1 == opt)
            break;
        switch (opt) {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf("option %s", long_options[option_index].name);

                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                break;
            case 'b':
                bootrom = std::string(optarg);
                if (access(bootrom.c_str(), F_OK) == -1) {
                    std::cout << "The bootrom file doesn't exist" << std::endl;
                    exit(1);
                }
                break;
            case 'r':
                rom = std::string(optarg);
                if (access(rom.c_str(), F_OK) == -1) {
                    std::cout << "The rom file doesn't exist" << std::endl;
                    exit(1);
                }
                break;
            case 'l':
                save_file = std::string(optarg);
                if (access(save_file.c_str(), F_OK) == -1) {
                    std::cout << "The save file doesn't exist" << std::endl;
                    exit(1);
                }
                break;
            default:
                abort();
        }
    }
    if (rom.empty()) {
        std::cout << "Missing rom argument" << std::endl;
        exit(1);
    }

    if (optind < argc) {
        printf("Invalid option: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        putchar('\n');
        exit(1);
    }

    GB gameboy;

    gameboy.init(rom, bootrom, save_file, debug_flag);
    gameboy.run();

    return 0;
}

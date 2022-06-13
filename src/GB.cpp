#include <iostream>
#include "gb.h"
#include <cstdint>
#include <unistd.h>
#include <set>
#include <chrono>
#include <thread>
#include <getopt.h>


void GB::init(std::string rom, std::string bootrom, bool debug){
    interrupts = new Interrupts(&registers, &mmu);
    cpu = new CPU(&registers, interrupts, &mmu);
    ppu = new PPU(&registers, interrupts, &mmu);
    timer = new Timer(&mmu, interrupts);
    joypad = new Joypad(&status, interrupts, &mmu);
    status.debug = debug;

    renderer = new Renderer(&status, cpu, ppu, &registers, &mmu);

    if(!bootrom.empty())
        mmu.load_boot_rom(bootrom);
    else
        cpu->no_bootrom_init();

    mmu.load_cartrige_rom(rom);
}

/* static struct timespec frameStart; */
/* struct timespec frameEnd; */
int counter = 0;
void GB::run(){
    while(status.isRunning) {
        if(!status.isPaused || status.doStep){
            mmu.clock.t_prev = mmu.clock.t;
            bool interrupted = interrupts->check();
            if(!interrupted)
                bool isstop = cpu->step();
            else
                cpu->instrs_count += 1;

            timer->inc();
            ppu->step();


            if (mmu.read_byte(0xff02) == 0x81) {
                char c = mmu.read_byte(0xff01);
                printf("%c", c);
                mmu.write_byte(0xff02, 0x0);
            }

            counter++;
        }

        if(ppu->can_render || status.doStep){
        // if(gpu->can_render){
            renderer->render();
            ppu->can_render = false;
            // static struct timespec frameStart;
            // struct timespec frameEnd;

            // long mtime, seconds, useconds;

            // clock_gettime(CLOCK_MONOTONIC, &frameEnd);
            // seconds = frameEnd.tv_sec - frameStart.tv_sec;
            // useconds = frameEnd.tv_nsec - frameStart.tv_nsec;

            // mtime = (seconds * 1000 + useconds / (1000.0 * 1000.0));

            // if(mtime < 1.0 / 60.0) 
            //     sleep(1 / 60.0 - mtime);

            // clock_gettime(CLOCK_MONOTONIC, &frameStart);
        }
        joypad->check(mmu.clock.t_instr);
        status.doStep = false;
    }
}

int main(int argc, char *argv[]){
    std::string rom = "";
    std::string bootrom = "";
    int debug_flag = 0;

    static struct option long_options[] ={
            {"debug",   no_argument,       &debug_flag, true},
            {"bootrom", required_argument, 0, 'b'},
            {"rom",     required_argument, 0, 'r'},
        };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long (argc, argv, "b:r:", long_options, &option_index)) != EOF){
        if (-1 == opt)
            break;
        switch (opt)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);

                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'b':
                bootrom = std::string(optarg);
                printf ("option -a with value `%s'\n", optarg);
                break;

            case 'r':
                rom = std::string(optarg);
                if( access( rom.c_str(), F_OK ) == -1 ){
                    std::cout << "The rom file doesn't exist" << std::endl;
                    exit(1);
                }
                // std::cout << "The file doesn't exist" << typeid(optarg).name << std::endl;

                printf ("option -b with value `%s'\n", optarg);
                break;
            default:
                abort ();
        }
    }
    if (optind < argc) {
        printf ("Invalid option: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    GB gameboy;
     
    gameboy.init(rom, bootrom, debug_flag);
    gameboy.run(); 
    
    return 0;
}

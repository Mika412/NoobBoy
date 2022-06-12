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
    gpu = new GPU(&registers, interrupts, &mmu);
    timer = new Timer(&mmu, interrupts);
    joypad = new Joypad(interrupts, &mmu);

    if(render)
        renderer = new Renderer(cpu, ppu, &registers, &mmu, debug);
    
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
    while(isRunning) {
        if(!isPaused || doStep){
            mmu.clock.t_prev = mmu.clock.t;
            bool interrupted = interrupts->check();
            if(!interrupted)
                bool isstop = cpu->step();
            else
                cpu->instrs_count += 1;

            if (mmu.read_byte(0xff02) == 0x81) {
                char c = mmu.read_byte(0xff01);
                printf("%c", c);
                mmu.write_byte(0xff02, 0x0);
            }





        if(counter>=10000){
        if(render)
            renderer->render();
        counter=0;
        }
        counter++;
        
        // long mtime, seconds, useconds;
        
        /* clock_gettime(CLOCK_MONOTONIC, &frameEnd);
        seconds = frameEnd.tv_sec - frameStart.tv_sec;
        useconds = frameEnd.tv_nsec - frameStart.tv_nsec;
        
        mtime = (seconds * 1000 + useconds / (1000.0 * 1000.0));
        
        if(mtime < 1.0 / 60.0) sleep(1 / 60.0 - mtime);
        
        clock_gettime(CLOCK_MONOTONIC, &frameStart); */
        // uint16_t tmp_bit = mmu->read_short(0x4244);
        // if(tmp_bit == 0xcb10){
            // std::cout << "BYTE IS " << std::hex << +tmp_bit << std::endl;
        //     exit(1);
        // }
        /* cpu->registers->print_flags();
        cpu->registers->print_registers(); */
         
        // renderer->render();    
        // if(isstop){
        // if(debug)
        //     if(isstop || stopped){
        //         cpu->registers->print_flags();
        //         cpu->registers->print_registers();
        //         std::cout << "LOCK IS " << std::hex << +mmu->clock.t << std::endl;
        //         std::cout << "DIV IS " << std::hex << +mmu->read_byte(0xff04) << std::endl;
        //         std::cout << "TAC IS " << std::hex << +mmu->read_byte(0xff07) << std::endl;

                // clock_gettime(CLOCK_MONOTONIC, &frameStart);
            }
            // std::cout << "AF: " << +registers.af << std::endl;
            // std::cout << "BC: " << +registers.bc << std::endl;
            // std::cout << "DE: " << +registers.de << std::endl;
            // std::cout << "HL: " << +registers.hl << std::endl;
            // std::cout << "SP: " << +registers.sp << std::endl;
            // std::cout << "PC: " << +(registers.pc - 0x100)<< std::endl;
            // std::cout << "JOYPAD: " << std::hex << +mmu.read_byte(0xff00) << std::endl;
            // std::cout << "\n" << std::endl;
            // cpu->registers->print_flags();
            // cpu->registers->print_registers();
            // if(registers.pc == 0xc2b5)
            //     exit(1);
            
            doStep = false;
        }
        joypad->check(mmu.clock.t_instr);
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

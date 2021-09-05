#include <iostream>
#include "gb.h"
#include <SDL2/SDL_keycode.h>
#include <cstdint>
#include <unistd.h>
#include <set>
#include <chrono>
#include <thread>

void GB::init(){
    memory = new MemoryBus(&registers);
    cpu = new CPU(&registers, memory);
    gpu = new GPU(&registers, memory);
    interrupt = new Interrupt(&registers, memory);
    timer = new Timer(&registers, memory);

    if(render)
        renderer = new Renderer(cpu, gpu, &registers, memory);

    memory->load_boot_rom("./roms/DMG_ROM.bin");
    // memory->load_cartrige_rom("./roms/Dr. Mario (World).gb");
    // memory->load_cartrige_rom("./roms/Pokemon Red.gb");
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/11-op a,(hl).gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/10-bit ops.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/09-op r,r.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/08-misc instrs.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/06-ld r,r.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/05-op rp.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/04-op r,imm.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/03-op sp,hl.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/02-interrupts.gb");
    // memory->load_cartrige_rom("./roms/cpu_instrs/individual/01-special.gb"); // PASSED
    // memory->load_cartrige_rom("./roms/instr_timing/instr_timing.gb");
    // memory->load_cartrige_rom("./roms/GB-TicTacToe.gb");
    // memory->load_cartrige_rom("./roms/daa.gb");
    // memory->load_cartrige_rom("./cpu_instrs.gb");
    // memory->load_cartrige_rom("./Tetris2.gb");
    memory->load_cartrige_rom("./roms/Tetris.gb");
}
    
/* static struct timespec frameStart; */
/* struct timespec frameEnd; */
std::set<int> myset;
std::set<int>::iterator it;
bool isTracking = false;
void GB::run(){
    int close = 0;
    int stopped = 0;
    while(!close) {
        bool isstop = cpu->step();    
        gpu->step();

        timer->inc();

        registers.m = 0;
        registers.t = 0;

        interrupt->step();

        registers.m = registers.t / 4;
        memory->clock.m += registers.m;
        memory->clock.t += registers.t;

        timer->inc();
        if(render)
            renderer->render();
        
        // long mtime, seconds, useconds;
        
        /* clock_gettime(CLOCK_MONOTONIC, &frameEnd);
        seconds = frameEnd.tv_sec - frameStart.tv_sec;
        useconds = frameEnd.tv_nsec - frameStart.tv_nsec;
        
        mtime = (seconds * 1000 + useconds / (1000.0 * 1000.0));
        
        if(mtime < 1.0 / 60.0) sleep(1 / 60.0 - mtime);
        
        clock_gettime(CLOCK_MONOTONIC, &frameStart); */
        // uint16_t tmp_bit = memory->read_short(0x4244);
        // if(tmp_bit == 0xcb10){
            // std::cout << "BYTE IS " << std::hex << +tmp_bit << std::endl;
        //     exit(1);
        // }
        /* cpu->registers->print_flags();
        cpu->registers->print_registers(); */
         
        // renderer->render();    
        // if(isstop){
        if(debug)
            if(isstop || stopped){
                cpu->registers->print_flags();
                cpu->registers->print_registers();
                std::cout << "LOCK IS " << std::hex << +memory->clock.t << std::endl;
                std::cout << "DIV IS " << std::hex << +memory->read_byte(0xff04) << std::endl;
                std::cout << "TAC IS " << std::hex << +memory->read_byte(0xff07) << std::endl;

                std::cout << "LY: " << std::hex << +this->memory->read_byte(0xff42) << std::endl;
                std::string dummy;
                std::cout << "Enter to continue..." << std::endl;
                std::getline(std::cin, dummy);
                stopped = 1;
            }
        if(isTracking){
            myset.insert(registers.pc);
            // myset.insert(cpu->last_instruction);
            std::cout << "myset contains:";
            for (it=myset.begin(); it!=myset.end(); ++it)
                std::cout << ' ' << *it;
              std::cout << '\n';
        }
        SDL_Event event;
        SDL_PollEvent(&event);
        // while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYUP:
                    switch(event.key.keysym.sym){
                        case SDLK_RIGHT:  memory->keys.keys2 &= 0xE; break;
                        case SDLK_LEFT:   memory->keys.keys2 &= 0xD; break;
                        case SDLK_UP:     memory->keys.keys2 &= 0xB; break;
                        case SDLK_DOWN:   memory->keys.keys2 &= 0x7; break;
                        case SDLK_z:      memory->keys.keys1 &= 0xE; break;
                        case SDLK_x:      memory->keys.keys1 &= 0xD; break;
                        case SDLK_SPACE:  memory->keys.keys1 &= 0xB; break;
                        case SDLK_RETURN: memory->keys.keys1 &= 0x7; break;
                        
                    }
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_RIGHT:  memory->keys.keys2 |= 0x1; break;
                        case SDLK_LEFT:   memory->keys.keys2 |= 0x2; break;
                        case SDLK_UP:     memory->keys.keys2 |= 0x4; break;
                        case SDLK_DOWN:   memory->keys.keys2 |= 0x8; break;
                        case SDLK_z:      memory->keys.keys1 |= 0x1; break;
                        case SDLK_x:      memory->keys.keys1 |= 0x2; break;
                        case SDLK_SPACE:  memory->keys.keys1 |= 0x4; break;
                        case SDLK_RETURN: memory->keys.keys1 |= 0x8; break;
                        case SDLK_r:      renderer->render(); break;
                        case SDLK_ESCAPE: exit(1); break;
                    }
                    break;
                case SDL_QUIT:
                    close = 1;
                    break;
            }
        // }
    }
}

int main(){
    GB gameboy;

    gameboy.init();
    gameboy.run(); 
    // gameboy.render(); 
    
    return 0;
}

// g++ main.cpp `pkg-config --cflags --libs sdl2`
/* #include <SDL.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

int main2( int argc, char** argv )
{
        SDL_Init( SDL_INIT_EVERYTHING );

    SDL_Window* window = SDL_CreateWindow
        (
            "SDL2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        600, 600,
        SDL_WINDOW_SHOWN
        );

    SDL_Renderer* renderer = SDL_CreateRenderer
        (
            window,
        -1,
        SDL_RENDERER_ACCELERATED
        );

    SDL_RendererInfo info;
    SDL_GetRendererInfo( renderer, &info );
    std::cout << "Renderer name: " << info.name << std::endl;
    std::cout << "Texture formats: " << std::endl;
    for( Uint32 i = 0; i < info.num_texture_formats; i++ )
    {
            std::cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << std::endl;
    }

    const unsigned int texWidth = 1024;
    const unsigned int texHeight = 1024;
    SDL_Texture* texture = SDL_CreateTexture
        (
            renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        texWidth, texHeight
        );

    std::vector< unsigned char > pixels( texWidth * texHeight * 4, 0 );

    SDL_Event event;
    bool running = true;
    bool useLocktexture = false;
    
    unsigned int frames = 0;
    Uint64 start = SDL_GetPerformanceCounter();

    while( running )
    {
    
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderClear( renderer );

        while( SDL_PollEvent( &event ) )
        {
                if( ( SDL_QUIT == event.type ) ||
                ( SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode ) )
            {
                    running = false;
                break;
            }
            if( SDL_KEYDOWN == event.type && SDL_SCANCODE_L == event.key.keysym.scancode )
            {
                    useLocktexture = !useLocktexture;
                std::cout << "Using " << ( useLocktexture ? "SDL_LockTexture() + memcpy()" : "SDL_UpdateTexture()" ) << std::endl;
            }
        }
        
        // splat down some random pixels
        for( unsigned int i = 0; i < 1000; i++ )
        {
                const unsigned int x = rand() % texWidth;
            const unsigned int y = rand() % texHeight;

            const unsigned int offset = ( texWidth * 4 * y ) + x * 4;
            pixels[ offset + 0 ] = rand() % 256;        // b
            pixels[ offset + 1 ] = rand() % 256;        // g
            pixels[ offset + 2 ] = rand() % 256;        // r
            pixels[ offset + 3 ] = SDL_ALPHA_OPAQUE;    // a
        }

        if( useLocktexture )
        {
                unsigned char* lockedPixels = nullptr;
            int pitch = 0;
            SDL_LockTexture
                (
                    texture,
                NULL,
                reinterpret_cast< void** >( &lockedPixels ),
                &pitch
                );
            std::memcpy( lockedPixels, pixels.data(), pixels.size() );
            SDL_UnlockTexture( texture );
        }
        else
        {
                SDL_UpdateTexture
                (
                    texture,
                NULL,
                pixels.data(),
                texWidth * 4
                );
        }

        SDL_RenderCopy( renderer, texture, NULL, NULL );
        SDL_RenderPresent( renderer );
        
        frames++;
        const Uint64 end = SDL_GetPerformanceCounter();
        const static Uint64 freq = SDL_GetPerformanceFrequency();
        const double seconds = ( end - start ) / static_cast< double >( freq );
        if( seconds > 2.0 )
        {
                std::cout
                << frames << " frames in "
                << std::setprecision(1) << std::fixed << seconds << " seconds = "
                << std::setprecision(1) << std::fixed << frames / seconds << " FPS ("
                << std::setprecision(3) << std::fixed << ( seconds * 1000.0 ) / frames << " ms/frame)"
                << std::endl;
            start = end;
            frames = 0;
        }
    }

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
*/

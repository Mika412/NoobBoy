# GameBoy Emulator (WIP)

A simple gameboy emulator that I'm writing to learn more about the world of emulation. This emulator has nothing special and has no extra features compared to other gb emulators.

![Running Tetris](screenshots/tetris.png)

## Usage
Only tested on linux. I have no idea about the dependecies and memory nibbles on other OSs.

    cmake .
    make
    ./GameBoy

    
## To-Dos

- [ ] Fix viewport rectangle overlap in the background view
- [ ] Fix clock cycle counter
- [ ] Fix timer interrupt
- [ ] Fix SDL2 input delay bug
- [ ] Receive joypad input on correct clock cycle
- [ ] Implement sprites
- [ ] Implement sound
- [ ] Refactor memory class
- [ ] Receive rom location as command line argument

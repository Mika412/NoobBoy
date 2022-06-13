#pragma once

enum ColorModes { 
    NORMAL,
    RETRO
};

struct Status {
    bool debug = false;

    bool isRunning = true;
    bool isPaused = false;
    bool doStep = false;
    ColorModes colorMode = RETRO;
};

#pragma once

enum ColorModes { 
    NORMAL = 0,
    RETRO = 1
};

struct Status {
    bool debug = false;

    bool isRunning = true;
    bool isPaused = false;
    bool doStep = false;
    int colorMode = NORMAL;
};

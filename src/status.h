#pragma once

enum ColorModes { 
    NORMAL = 0,
    RETRO = 1,
    GRAY = 2
};

struct Status {
    bool debug = false;

    bool isRunning = true;
    bool isPaused = false;
    bool doStep = false;
    bool soundEnabled = true;

    int colorMode = NORMAL;
};

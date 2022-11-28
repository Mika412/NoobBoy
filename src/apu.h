#pragma once

#include <SDL2/SDL.h>
#include "mmu.h"
#include <vector>

union NRx1{
    struct {
        uint8_t length_load : 6;
        uint8_t duty : 2;
    } ;
    uint8_t value;
};

struct Frequency{
    union NRx3{
        struct {
            uint8_t frequency_lsb : 8;
        };
        uint8_t value;
    } nrx3;

    union NRx4{
        struct {
            uint8_t frequency_msb : 3;
            uint8_t empty: 3;
            uint8_t length_enable : 1;
            uint8_t trigger : 1;
        };
        uint8_t value;
    } nrx4;

    int duty = 0;
    int freq_timer = 0;
};

struct Envelop{
    union NRx2{
        struct {
            uint8_t period : 3;
            uint8_t direction : 1;
            uint8_t init_vol : 4;
        };
        uint8_t value;
    } nrx2;
    int volume = 0;
    int timer = 0;
};

struct Wave {
    union NR30 {
        struct {
            uint8_t unused : 7;
            uint8_t dac_power : 1;
        };
        uint8_t value;
    } nr30;
    uint8_t sample = 0;
};

class APU {
public:
    MMU *mmu;

    uint8_t duties[4][8] = {
        {0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,1,1},
        {0,1,1,1,1,1,1,0}
    };

    int audio_freq = 44100;
    int gameboy_ticks = 4 * 1024 * 1024; // 4194304
    int sample_rate = gameboy_ticks / audio_freq;

    Frequency ch1_frequency;
    Frequency ch2_frequency;
    Frequency ch3_frequency;
    NRx1 nr11;
    NRx1 nr21;
    
    // Channels
    Wave wave;

    APU(MMU *mmu);

    void init_audio();

    uint8_t get_ch1_sample();
    uint8_t get_ch2_sample();
    uint8_t get_ch3_sample();
    uint8_t get_next_sample();

    // Actions
    uint8_t timer_action(Frequency *envelop);
    uint8_t duty_action(Frequency *envelop, NRx1 *nrx1, uint8_t stepped_timer);
};

void audio_callback(void *, uint8_t *, int);

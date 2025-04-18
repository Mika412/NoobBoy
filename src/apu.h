#pragma once

#include "mmu.h"
#include "status.h"

#include <SDL3/SDL.h>
#include <vector>

union NRx1 {
    struct {
        uint8_t length_load : 6;
        uint8_t duty : 2;
    };
    uint8_t value;
};

struct Frequency {
    union NRx3 {
        struct {
            uint8_t frequency_lsb : 8;
        };
        uint8_t value;
    } nrx3;

    union NRx4 {
        struct {
            uint8_t frequency_msb : 3;
            uint8_t empty : 3;
            uint8_t length_enable : 1;
            uint8_t trigger : 1;
        };
        uint8_t value;
    } nrx4;

    uint8_t duty = 0;
    uint16_t freq_timer = 0;
};

struct FrameSequencer {
    int frame_timer;
    int frame_sequence;

    int length_timer = 0;

    bool trigger_length = 0;
    bool trigger_envelope = 0;
    bool trigger_sweep = 0;
};

struct Envelope {
    union NRx2 {
        struct {
            uint8_t period : 3;
            uint8_t direction : 1;
            uint8_t init_vol : 4;
        };
        uint8_t value;
    } nrx2;
    int volume = 0;
    int timer = 0;
    bool enabled = false;
};

struct Wave {
    union NR30 {
        struct {
            uint8_t unused : 7;
            uint8_t dac_power : 1;
        };
        uint8_t value;
    } nr30;
    union NR31 {
        struct {
            uint16_t length_load : 8;
        };
        uint8_t value;
    } nr31;
    union NR32 {
        struct {
            uint8_t empty : 5;
            uint8_t volume : 2;
            uint8_t empty2 : 1;
        };
        uint8_t value;
    } nr32;
    FrameSequencer sequencer;
    Frequency frequency;
    uint8_t sample = 0;

    bool enabled;
};

struct Square {
    FrameSequencer sequencer;
    Envelope envelope;
    Frequency frequency;

    NRx1 nrx1;
    bool enabled;
};

struct Noise {
    union NR43 {
        struct {
            uint8_t divisor : 3;
            uint8_t width_mode : 1;
            uint8_t clock_shift : 4;
        };
        uint8_t value;
    } nr43;
    FrameSequencer sequencer;
    Envelope envelope;
    Frequency frequency;

    NRx1 nrx1;
    uint16_t lfsr;

    bool enabled;
};

class APU {
   public:
    Status *status;
    MMU *mmu;

    uint8_t duties[4][8] = {
        {0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 1, 1, 1}, {0, 1, 1, 1, 1, 1, 1, 0}};
    uint8_t divisor[8] = {8, 16, 32, 48, 64, 80, 96, 112};

    int audio_freq = 44100;
    int gameboy_ticks = 4 * 1024 * 1024;  // 4194304
    int sample_rate = gameboy_ticks / audio_freq;

    // Channels
    Square ch1;
    Square ch2;
    Wave wave;
    Noise noise;
    Noise ch4;

    APU(Status *status, MMU *mmu);

    void init_audio();

    uint8_t get_ch1_sample();
    uint8_t get_ch2_sample();
    uint8_t get_ch3_sample();
    uint8_t get_ch4_sample();

    uint8_t get_next_sample();

    void reset_ch1();
    void reset_ch2();
    void reset_ch3();
    void reset_ch4();

    // Actions
    void frame_sequencer_action(FrameSequencer *fs);
    bool timer_action(Frequency *frequency);
    void duty_action(Frequency *frequency);
    bool length_timer_action(Frequency *frequency, FrameSequencer *fs);
    void envelope_action(Envelope *envelope, FrameSequencer *fs);
};

void audio_callback(void *, SDL_AudioStream *, int, int);
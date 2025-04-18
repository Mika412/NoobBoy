
#include "apu.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdio.h>
#include <unistd.h>

APU::APU(Status *status, MMU *mmu) {
    this->status = status;
    this->mmu = mmu;
    init_audio();
}

void APU::init_audio() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    const SDL_AudioSpec spec = {SDL_AUDIO_U8, 2, audio_freq};
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_callback, this);
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
}

void APU::frame_sequencer_action(FrameSequencer *sequencer) {
    sequencer->frame_timer++;
    if (sequencer->frame_timer == 8192) {
        sequencer->frame_timer = 0;
        sequencer->frame_sequence++;
        sequencer->frame_sequence %= 8;

        sequencer->trigger_length = sequencer->frame_sequence % 2 == 0;
        sequencer->trigger_envelope = sequencer->frame_sequence == 7;
        sequencer->trigger_sweep = sequencer->frame_sequence == 2 || sequencer->frame_sequence == 6;
    } else {
        sequencer->trigger_length = false;
        sequencer->trigger_envelope = false;
        sequencer->trigger_sweep = false;
    }
}

bool APU::timer_action(Frequency *frequency) {
    if (frequency->freq_timer <= 0) {
        uint16_t wavelen = ((frequency->nrx4.frequency_msb << 8) | frequency->nrx3.frequency_lsb);
        frequency->freq_timer = 4 * (2048 - wavelen);
        return true;
    } else
        frequency->freq_timer--;
    return false;
}

void APU::duty_action(Frequency *frequency) {
    frequency->duty++;
    frequency->duty %= 8;
}

bool APU::length_timer_action(Frequency *frequency, FrameSequencer *sequencer) {
    if (sequencer->trigger_length && frequency->nrx4.length_enable && sequencer->length_timer) {
        sequencer->length_timer--;
        if (sequencer->length_timer <= 0)
            return false;
    }
    return true;
}

void APU::envelope_action(Envelope *envelope, FrameSequencer *sequencer) {
    if (sequencer->trigger_envelope && envelope->enabled && envelope->nrx2.period) {
        envelope->timer--;
        if (envelope->timer <= 0) {
            envelope->timer = envelope->nrx2.period;
            int direction = envelope->nrx2.direction ? 1 : -1;
            int new_volume = envelope->volume + direction;
            if (new_volume >= 0 && new_volume <= 0x0F)
                envelope->volume = new_volume;
            else
                envelope->enabled = false;
        }
    }
}

// Sweep -> Timer -> Duty -> Length Counter -> Envelope -> Mixer
uint8_t APU::get_ch1_sample() {
    // Load all options from memory
    // mmu->read_byte(0xFF15)// NR20
    ch1.nrx1.value = mmu->read_byte(0xFF11);            // NR11
    ch1.envelope.nrx2.value = mmu->read_byte(0xFF12);   // NR12
    ch1.frequency.nrx3.value = mmu->read_byte(0xFF13);  // NR13
    ch1.frequency.nrx4.value = mmu->read_byte(0xFF14);  // NR14

    if (ch1.frequency.nrx4.trigger)
        reset_ch1();

    bool triggered_timer = timer_action(&ch1.frequency);
    if (triggered_timer)
        duty_action(&ch1.frequency);
    frame_sequencer_action(&ch1.sequencer);

    ch1.enabled &= length_timer_action(&ch1.frequency, &ch1.sequencer);

    if (ch1.sequencer.trigger_envelope)
        envelope_action(&ch1.envelope, &ch1.sequencer);

    uint8_t sample = duties[ch1.nrx1.duty][ch1.frequency.duty];
    return sample * ch1.envelope.volume * ch1.enabled;
}

// Timer -> Duty -> Length Counter -> Envelope -> Mixer
uint8_t APU::get_ch2_sample() {
    // Load all options from memory
    // mmu->read_byte(0xFF15)// NR20
    ch2.nrx1.value = mmu->read_byte(0xFF16);            // NR21
    ch2.envelope.nrx2.value = mmu->read_byte(0xFF17);   // NR22
    ch2.frequency.nrx3.value = mmu->read_byte(0xFF18);  // NR23
    ch2.frequency.nrx4.value = mmu->read_byte(0xFF19);  // NR24

    if (ch2.frequency.nrx4.trigger)
        reset_ch2();

    // Execute all the channel actions
    bool triggered_timer = timer_action(&ch2.frequency);
    if (triggered_timer)
        duty_action(&ch2.frequency);

    frame_sequencer_action(&ch2.sequencer);

    ch2.enabled &= length_timer_action(&ch2.frequency, &ch2.sequencer);

    if (ch2.sequencer.trigger_envelope)
        envelope_action(&ch2.envelope, &ch2.sequencer);

    // Create sample
    uint8_t sample = duties[ch2.nrx1.duty][ch2.frequency.duty];
    return sample * ch2.envelope.volume * ch2.enabled;
}

// Wave: Timer -> Wave -> Length Counter -> Volume -> Mixer
uint8_t APU::get_ch3_sample() {
    // Load all options from memory
    wave.nr30.value = mmu->read_byte(0xFF1A);            // NR30
    wave.nr31.value = mmu->read_byte(0xFF1B);            // NR32
    wave.nr32.value = mmu->read_byte(0xFF1C);            // NR32
    wave.frequency.nrx3.value = mmu->read_byte(0xFF1D);  // NR33
    wave.frequency.nrx4.value = mmu->read_byte(0xFF1E);  // NR34

    // Reset channel state
    if (wave.frequency.nrx4.trigger)
        reset_ch3();

    // Execute all the channel actions
    frame_sequencer_action(&wave.sequencer);

    // Timer action
    bool triggered_timer = timer_action(&wave.frequency);

    // Wave action
    if (triggered_timer)
        ++wave.sample %= 32;

    uint8_t sample = mmu->read_byte(0xff30 + (wave.sample / 2));
    if (wave.sample % 2)
        sample = sample & 0xf;
    else
        sample = sample >> 4;

    // Length Counter action
    wave.enabled &= length_timer_action(&wave.frequency, &wave.sequencer);

    // Volume action
    int shift_vol = wave.nr32.volume ? wave.nr32.volume - 1 : 4;
    sample >>= shift_vol;

    return sample * wave.enabled * wave.nr30.dac_power;
}

// Noise: Timer -> LFSR -> Length Counter -> Envelope -> Mixer
uint8_t APU::get_ch4_sample() {
    // Load all options from memory
    noise.nrx1.value = mmu->read_byte(0xFF20);            // NR41
    noise.envelope.nrx2.value = mmu->read_byte(0xFF21);   // NR42
    noise.nr43.value = mmu->read_byte(0xFF22);            // NR43
    noise.frequency.nrx4.value = mmu->read_byte(0xFF23);  // NR44

    if (noise.frequency.nrx4.trigger)
        reset_ch4();

    // Execute all the channel actions
    frame_sequencer_action(&noise.sequencer);

    // Timer and LFSR action
    noise.frequency.freq_timer--;
    if (noise.frequency.freq_timer <= 0) {
        noise.frequency.freq_timer = divisor[noise.nr43.divisor] << noise.nr43.clock_shift;
        uint8_t xor_res = (noise.lfsr & 0x1) ^ ((noise.lfsr & 0x2) >> 1);
        noise.lfsr = (noise.lfsr >> 1) | (xor_res << 14);

        if (noise.nr43.width_mode) {
            noise.lfsr &= ~(1 << 6);
            noise.lfsr |= (xor_res << 6);
        }
    }

    // Length Counter action
    noise.enabled &= length_timer_action(&noise.frequency, &noise.sequencer);

    // Envelope action
    envelope_action(&noise.envelope, &noise.sequencer);

    uint8_t sample = (~noise.lfsr & 0x1) * noise.envelope.volume;
    return sample * noise.enabled;
}

void APU::reset_ch1() {
    ch1.frequency.nrx4.trigger = 0;
    mmu->write_byte(0xFF14, ch1.frequency.nrx4.value);

    ch1.envelope.volume = ch1.envelope.nrx2.init_vol;
    ch1.enabled = true;
    if (ch1.nrx1.length_load)
        ch1.sequencer.length_timer = 64 - ch1.nrx1.length_load;
    ch1.envelope.enabled = true;
    // ch1.envelope.timer = ch1.envelope.nrx2.period;
}

void APU::reset_ch2() {
    // Reset trigger
    ch2.frequency.nrx4.trigger = 0;
    mmu->write_byte(0xFF19, ch2.frequency.nrx4.value);

    ch2.enabled = true;

    if (!ch2.sequencer.length_timer)
        ch2.sequencer.length_timer = 64 - ch2.nrx1.length_load;

    ch2.envelope.volume = ch2.envelope.nrx2.init_vol;
    ch2.envelope.enabled = true;
}

void APU::reset_ch3() {
    // Reset trigger
    wave.frequency.nrx4.trigger = 0;
    mmu->write_byte(0xFF1E, wave.frequency.nrx4.value);

    wave.enabled = true;
    wave.sample = 0;
    if (!wave.sequencer.length_timer)
        wave.sequencer.length_timer = 256 - wave.nr31.length_load;
}

void APU::reset_ch4() {
    // Reset trigger
    noise.frequency.nrx4.trigger = 0;
    mmu->write_byte(0xFF23, noise.frequency.nrx4.value);

    if (!noise.sequencer.length_timer)
        noise.sequencer.length_timer = 64 - noise.nrx1.length_load;

    noise.enabled = true;

    noise.lfsr = 0x7fff;
    noise.envelope.volume = noise.envelope.nrx2.init_vol;
    noise.envelope.enabled = true;
}

uint8_t APU::get_next_sample() {
    uint8_t ch1, ch2, ch3, ch4 = 0;

    for (int i = 0; i <= sample_rate; i++) {
        ch1 = get_ch1_sample();
        ch2 = get_ch2_sample();
        ch3 = get_ch3_sample();
        ch4 = get_ch4_sample();
    }

    if (status->isPaused || !status->soundEnabled)
        return 0x00;

    return ch1 + ch2 + ch3 + ch4;
}

void audio_callback(void *_sound, SDL_AudioStream *_stream, int _additional_amount, int _length) {
    uint8_t *stream = (uint8_t *)_stream;
    APU *sound = (APU *)_sound;
    int length = _length / sizeof(stream[0]);

    uint8_t *data = SDL_stack_alloc(uint8_t, _length);
    for (int i = 0; i < _length; i += 2) {
        // TODO: Change to support stereo properly
        uint8_t sample = sound->get_next_sample();
        data[i] = sample;
        data[i + 1] = sample;
    }
    SDL_PutAudioStreamData(_stream, data, _length);
    SDL_stack_free(data);
}


#include "apu.h"
#include <algorithm>
#include <stdio.h>
#include <cmath>

APU::APU(MMU* mmu){
    this->mmu = mmu;
    init_audio();
}

void APU::init_audio() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_AudioSpec audio_spec, obtainedSpec;
    SDL_zero(audio_spec);
    audio_spec.freq = audio_freq;
    audio_spec.format = AUDIO_U8;
    audio_spec.channels = 2;
    audio_spec.samples = audio_freq / 60;
    audio_spec.callback = audio_callback;
    audio_spec.userdata = this;

    SDL_OpenAudio(&audio_spec, &obtainedSpec);
    SDL_PauseAudio(0);
}


uint8_t APU::timer_action(Frequency *frequency){
    uint16_t wavelen = ((frequency->nrx4.frequency_msb << 8) | frequency->nrx3.frequency_lsb);
    uint16_t freq = 4 * (2048-wavelen);
    uint16_t stepped_timer = frequency->freq_timer + sample_rate;

    frequency->freq_timer = stepped_timer % freq;
    return stepped_timer / freq;
}

uint8_t APU::duty_action(Frequency *frequency, NRx1 *nrx1, uint8_t stepped_timer){
    frequency->duty += stepped_timer;
    frequency->duty %= 8;
    return duties[nrx1->duty][frequency->duty];
}

// Sweep -> Timer -> Duty -> Length Counter -> Envelope -> Mixer
uint8_t APU::get_ch1_sample() {
    // mmu->read_byte(0xFF15)// NR20
    nr11.value = mmu->read_byte(0xFF11); // NR11
    ch1_frequency.nrx3.value = mmu->read_byte(0xFF13); // NR13
    ch1_frequency.nrx4.value = mmu->read_byte(0xFF14); // NR14
    //
    uint8_t stepped_timer = timer_action(&ch1_frequency);
    uint8_t sample = duty_action(&ch1_frequency, &nr11, stepped_timer);
    return sample * 0xFF;
}

// Timer -> Duty -> Length Counter -> Envelope -> Mixer
uint8_t APU::get_ch2_sample() {
    // mmu->read_byte(0xFF15)// NR20
    nr21.value = mmu->read_byte(0xFF16); // NR21
    ch2_frequency.nrx3.value = mmu->read_byte(0xFF18); // NR23
    ch2_frequency.nrx4.value = mmu->read_byte(0xFF19); // NR24
   

    uint8_t stepped_timer = timer_action(&ch2_frequency);
    uint8_t sample = duty_action(&ch2_frequency, &nr21, stepped_timer);
    return sample * 0xFF;
}

// Wave: Timer -> Wave -> Length Counter -> Volume -> Mixer
uint8_t APU::get_ch3_sample() {
    wave.nr30.value = mmu->read_byte(0xFF1A);
    wave.nr32.value = mmu->read_byte(0xFF1C); // NR32
    ch3_frequency.nrx3.value = mmu->read_byte(0xFF1D); // NR33
    ch3_frequency.nrx4.value = mmu->read_byte(0xFF1E); // NR34
    
    if(!wave.nr30.dac_power) {
        wave.sample = 0;
        return 0x00;
    }

    uint8_t stepped_timer = timer_action(&ch3_frequency);

    // Wave action
    wave.sample += stepped_timer;
    wave.sample %= 32;

    uint8_t sample = mmu->read_byte(0xFF30 + wave.sample); // until 0xFF3F
    
    if(wave.sample % 2)
        sample = (sample & 0x0F) << 4;
    else
        sample &= 0xF0;
    
    // Volume action
    if(wave.nr32.volume == 0)
        sample = 0;
    else
        sample >>= wave.nr32.volume - 1;

    return sample * 0x0F;
}

uint8_t APU::get_next_sample() {
	if ((mmu->read_byte(0xff12) >> 3) == 0x0)
		return 0x00;

    uint8_t ch1 = get_ch1_sample();
    uint8_t ch2 = get_ch2_sample();
    uint8_t ch3 = get_ch3_sample();

    return (ch1+ch2+ch3)/3;
}

void audio_callback(void *_sound, uint8_t *_stream, int _length) {
    uint8_t *stream = (uint8_t *)_stream;
    APU *sound = (APU *)_sound;
    int length = _length / sizeof(stream[0]);
    for(int i = 0; i < length; i+=2) {
        // TODO: Change to support stereo properly
        uint8_t sample = sound->get_next_sample();
        stream[i] = sample;
        stream[i + 1] = sample;
    }
}

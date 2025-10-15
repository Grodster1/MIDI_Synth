#include "audio_engine.h"
#include "main.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 256

static float phase = 0.0f;
static float phase_step = 0.0f;
static uint8_t is_playing = 0;
static uint16_t audio_buffer[AUDIO_BUFFER_SIZE];

// Deklaracje zewnętrzne
extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim6;

void AudioEngine_Init(void){
    phase = 0.0f;
    is_playing = 0;
    phase_step = 0.0f;

    // Clear audio buffer
    memset(audio_buffer, 0, sizeof(audio_buffer));

    // Start DAC with DMA
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)audio_buffer, AUDIO_BUFFER_SIZE, DAC_ALIGN_12B_R);

    // Start timer for sample rate
    HAL_TIM_Base_Start(&htim6);
}

void AudioEngine_PlayTestTone(void){
    phase_step = 440.0f / SAMPLE_RATE; // A4 440 Hz
    is_playing = 1;
}

void AudioEngine_PlayNote(float frequency){
    phase_step = frequency / SAMPLE_RATE;
    is_playing = 1;
}

void AudioEngine_Stop(void){
    is_playing = 0;
}

void AudioEngine_ProcessBuffer(void){
    fill_audio_buffer(audio_buffer, AUDIO_BUFFER_SIZE);
}

void fill_audio_buffer(uint16_t* buffer, uint16_t size){
    for(uint16_t i = 0; i < size; i++){
        if(!is_playing){
            buffer[i] = 2048; // Silence (middle point for 12-bit DAC)
            continue;
        }

        float sample = sinf(2.0f * M_PI * phase);

        // Convert to 12-bit DAC range (0-4095)
        buffer[i] = 2048 + (uint16_t)(sample * 1000.0f);

        phase += phase_step;

        if(phase >= 1.0f){
            phase -= 1.0f;
        }
    }
}

void HAL_DAC_ConvCpltCallbackCh2(DAC_HandleTypeDef* hdac_param){
    AudioEngine_ProcessBuffer();
}

void HAL_DAC_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef* hdac_param){
    AudioEngine_ProcessBuffer();
}

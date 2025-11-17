#include "audio_engine.h"
#include "main.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 256
#define MAX_AMPLITUDE 2000.0f

typedef struct{
	uint8_t active;
	uint8_t note;
	float phase;
	float phase_step;
	uint32_t start_time;
} Voice;

static Voice voices[MAX_VOICES];
static uint16_t audio_buffer[AUDIO_BUFFER_SIZE];

// Deklaracje zewnętrzne
extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim6;




void AudioEngine_Init(void){
    // Clear voices
    memset(voices, 0, sizeof(voices));

    for(int i = 0; i < AUDIO_BUFFER_SIZE; i++){
        audio_buffer[i] = 2048; // Silence (middle point for 12-bit DAC)
    }

    // Start DAC with DMA
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)audio_buffer, AUDIO_BUFFER_SIZE, DAC_ALIGN_12B_R);

    // Start timer for sample rate
    HAL_TIM_Base_Start(&htim6);
}

static Voice* find_free_voice(uint8_t note){
	Voice* free_voice = NULL;
	for(int i = 0; i < MAX_VOICES; ++i){
		if(!voices[i].active){
			return &voices[i];
		}
	}

	uint32_t current_time = HAL_GetTick();
	uint32_t max_duration = 0;
	uint8_t oldest_voice_index = 0;
	for(int j = 0; j < MAX_VOICES; ++j){
		uint32_t duration = current_time - voices[j].start_time;
		if(duration > max_duration){
			max_duration = duration;
			oldest_voice_index = j;
		}
	}

	return &voices[oldest_voice_index];
}


void AudioEngine_PlayNote(uint8_t note, float frequency){
    Voice* voice = find_free_voice(note);
    voice->active = 1;
    voice->note = note;
    voice->phase = 0.0f;
    voice->phase_step = voice->phase/SAMPLE_RATE;
    voice->start_time = HAL_GetTick();
}

void AudioEngine_Stop(uint8_t note){
    for(int i = 0; i < MAX_VOICES; ++i){
    	if(voices[i].active && voices[i].note == note){
    		voices[i].active = 0;
    		break;
    	}
    }
}

void AudioEngine_ProcessBuffer(void){
    fill_audio_buffer(audio_buffer, AUDIO_BUFFER_SIZE);
}

void fill_audio_buffer(uint16_t* buffer, uint16_t size){

	for(int i = 0; i < size; ++i){
		float mixed_sample = 0.0f;    //Sinusoid sum

		for(int j = 0; j < MAX_VOICES; ++j){
			if(voices[i].active){
				mixed_sample += sinf(2.0f * M_PI * voices[i].phase);
				voices[i].phase += voices[i].phase_step;
				if(voices[i].phase >= 1.0f){
					voices[i].phase -= 1.0f;
				}
			}
		}
		mixed_sample = (mixed_sample / MAX_VOICES) * MAX_AMPLITUDE;
		buffer[i] = 2048 + (uint16_t)(mixed_sample);
	}
}

void HAL_DAC_ConvCpltCallbackCh2(DAC_HandleTypeDef* hdac_param){
    AudioEngine_ProcessBuffer();
}

void HAL_DAC_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef* hdac_param){
    AudioEngine_ProcessBuffer();
}

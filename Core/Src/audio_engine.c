#include "audio_engine.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 256
#define MAX_AMPLITUDE 800.0f
#define RELEASE_SPEED 0.005f
#define MAX_VOICES 8

typedef struct{
	float rate;
	float phase;
	float phase_step;
	float depth;
	WaveType LFO_wave;
	LFOMode mode;
} LFO;


typedef struct{
	uint8_t active;
	uint8_t note;
	uint8_t note_on;
	float phase;
	float phase_step;
	float amplitude;
	uint32_t start_time;
} Voice;

static LFO lfo;
static Voice voices[MAX_VOICES];
static uint16_t audio_buffer[AUDIO_BUFFER_SIZE];
static WaveType current_wave_type = SIN_WAVE;
static WaveType current_LFO_wave_type = SIN_WAVE;

extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim6;


void AudioEngine_Init(void){
    // Clear voices
    memset(voices, 0, sizeof(voices));

    //DODANO LFO
    lfo.rate = 2.0f;
    lfo.phase = 0.0f;
    lfo.phase_step = lfo.rate/(float)SAMPLE_RATE;
    lfo.depth = 0.1f;
    lfo.LFO_wave = current_LFO_wave_type;
    lfo.mode = TREMOLO;


    for(int i = 0; i < AUDIO_BUFFER_SIZE; i++){
        audio_buffer[i] = 2048; // Silence (middle point for 12-bit DAC)
    }

    __HAL_DAC_ENABLE_IT(&hdac, DAC_IT_DMAUDR2);

    if (HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)audio_buffer, AUDIO_BUFFER_SIZE, DAC_ALIGN_12B_R) != HAL_OK)
    {
        HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
        printf("ERROR: DAC DMA Start failed!\r\n");
    }
    else {
        printf("DAC DMA Started successfully\r\n");
    }

    printf("Adres bufora: %p\r\n", (void*)audio_buffer);

    if ((uint32_t)audio_buffer < 0x20000000) {
        printf("ERROR: Bufor jest w CCM RAM! DMA tego nie obsłuży!\r\n");
    } else {
        printf("OK: Bufor jest w SRAM.\r\n");
    }

    HAL_TIM_Base_Start(&htim6);
    printf("TIM6 Started\r\n");
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
    voice->note_on = 1;
    voice->phase = 0.0f;
    voice->amplitude = 1.0f;
    voice->phase_step = frequency / (float)SAMPLE_RATE;
    voice->start_time = HAL_GetTick();
}

void AudioEngine_StopNote(uint8_t note){
    for(int i = 0; i < MAX_VOICES; ++i){
    	if(voices[i].active && voices[i].note == note && voices[i].note_on){
    		voices[i].note_on = 0;
    		break;
    	}
    }
}

float AudioEngine_MIDINoteToFrequency(uint8_t note){
	return 440.0f*powf(2.0f, (note - 69.0f)/12.0f);
}

void AudioEngine_ProcessBuffer(void){
    fill_audio_buffer(audio_buffer, AUDIO_BUFFER_SIZE);
}

void AudioEngine_SetWaveType(WaveType type){
	current_wave_type = type;
}

void AudioEngine_SetLFOWaveType(WaveType type){
	current_LFO_wave_type = type;
}






/* Funkcja fill_audio_buffer zapełnia bufor danych wartościami spróbkowanej fali.
 * Wartość phase przyjmuje wartości od 0 do 1 (jeden pełen okres).
 *
 */
void fill_audio_buffer(uint16_t* buffer, uint16_t size){
    for(int i = 0; i < size; ++i){
        float mixed_sample = 0.0f;
        int active_voices_count = 0;
        float lfo_val = 0.0f;
        float vibrato_mod = 1.0f;
        float tremolo_mod = 1.0f;
        //LFO
        switch(current_LFO_wave_type){
            case SIN_WAVE:
            	lfo_val = sinf(2.0f*M_PI*lfo.phase);
                break;
            case SQUARE_WAVE:
            	lfo_val = (lfo.phase < 0.5f) ? 1.0f : -1.0f;
                break;
            case SAWTOOTH_WAVE:
            	lfo_val = (2.0f * lfo.phase) - 1.0f;
                break;
            case TRIANGLE_WAVE:
                if(lfo.phase < 0.5f){
                	lfo_val = 4.0f * lfo.phase - 1.0f;
                }
                else{
                	lfo_val = -4.0f * lfo.phase + 3.0f;
                }
                break;
        }
        if(lfo.mode == VIBRATO){
        	vibrato_mod = 1.0f + (lfo_val * lfo.depth);
        }
        else if(lfo.mode == TREMOLO){
        	tremolo_mod = ((lfo_val*lfo.depth + 1.0) / 2.0);
        }

        //DODANE LFO
        lfo.phase += lfo.phase_step;
        if(lfo.phase >= 1.0f){
        	lfo.phase -= 1.0f;
        }

        for(int j = 0; j < MAX_VOICES; ++j){
            if(voices[j].active && voices[j].amplitude > 0.0f){
                active_voices_count++;
            }
        }
        float normalization = (active_voices_count > 0) ?
                              (1.0f / sqrtf((float)active_voices_count)) : 1.0f;



        for(int j = 0; j < MAX_VOICES; ++j){
            if(voices[j].active){
                // Envelope (Release)
                if(voices[j].note_on){
                    voices[j].amplitude = 1.0f;
                }
                else{
                    voices[j].amplitude -= RELEASE_SPEED;
                    if(voices[j].amplitude <= 0.0f){
                        voices[j].amplitude = 0.0f;
                        voices[j].active = 0;
                        continue;
                    }
                }

                float phase = voices[j].phase;
                float sample_val = 0.0f;

                switch(current_wave_type){
                    case SIN_WAVE:
                        sample_val = sinf(2.0f * M_PI * phase);
                        break;
                    case SQUARE_WAVE:
                        sample_val = (phase < 0.5f) ? 1.0f : -1.0f;
                        break;
                    case SAWTOOTH_WAVE:
                        sample_val = (2.0f * phase) - 1.0f;
                        break;
                    case TRIANGLE_WAVE:
                        if(phase < 0.5f){
                            sample_val = 4.0f * phase - 1.0f;
                        }
                        else{
                            sample_val = -4.0f * phase + 3.0f;
                        }
                        break;
                }

                sample_val *= voices[j].amplitude;

                //TREMOLO MODULATION
                sample_val *= tremolo_mod;
                sample_val *= normalization;
                sample_val *= MAX_AMPLITUDE;

                mixed_sample += sample_val;

                //VIBRATO MODULATION
                float current_step = voices[j].phase_step * vibrato_mod;

                // Aktualizuj fazę
                voices[j].phase += current_step;
                if(voices[j].phase >= 1.0f){
                    voices[j].phase -= 1.0f;
                }
            }
        }

        float normalized = mixed_sample / MAX_AMPLITUDE;

        normalized = tanhf(normalized);

        mixed_sample = normalized * MAX_AMPLITUDE;

        int32_t final_value = 2048 + (int32_t)mixed_sample;

        if (final_value > 4095) final_value = 4095;
        else if (final_value < 0) final_value = 0;

        buffer[i] = (uint16_t)final_value;
    }
}

float AudioEngine_GetLFORate(void) {
    return lfo.rate;
}

float AudioEngine_GetLFODepth(void) {
    return lfo.depth;
}

LFOMode AudioEngine_GetLFOMode(void) {
    return lfo.mode;
}

WaveType AudioEngine_GetLFOWaveType(void) {
    return lfo.LFO_wave;
}

const char* AudioEngine_GetLFOWaveName(WaveType mode){
	switch(mode){
	case SIN_WAVE:
		return "SIN";
	case SQUARE_WAVE:
		return "SQUARE";
	case SAWTOOTH_WAVE:
		return "SAWTOOTH";
	case TRIANGLE_WAVE:
		return "TRIANGLE";
	}
}

const char* AudioEngine_GetLFOModeName(LFOMode mode){
	switch(mode){
	case TREMOLO:
		return "TREMOLO";
	case VIBRATO:
		return "VIBRATO";
	}
}
void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);

    fill_audio_buffer(&audio_buffer[0], AUDIO_BUFFER_SIZE / 2);
}

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    fill_audio_buffer(&audio_buffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2);
}

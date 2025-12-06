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
#define MAX_AMPLITUDE 1800.0f

typedef struct{
	uint8_t active;
	uint8_t note;
	float phase;
	float phase_step;
	uint32_t start_time;
} Voice;

static Voice voices[MAX_VOICES];
static uint16_t audio_buffer[AUDIO_BUFFER_SIZE];
static WaveType current_wave_type = SIN_WAVE;
// Deklaracje zewnętrzne
extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim6;


void AudioEngine_Init(void){
    // Clear voices
    memset(voices, 0, sizeof(voices));

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
    voice->phase = 0.0f;
    voice->phase_step = frequency / (float)SAMPLE_RATE;
    voice->start_time = HAL_GetTick();
}

void AudioEngine_StopNote(uint8_t note){
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

void AudioEngine_SetWaveType(WaveType type){
	current_wave_type = type;
}




/* Funkcja fill_audio_buffer zapełnia bufor danych wartościami spróbkowanej fali.
 * Wartość phase przyjmuje wartości od 0 do 1 (jeden pełen okres).
 *
 */
void fill_audio_buffer(uint16_t* buffer, uint16_t size){

	for(int i = 0; i < size; ++i){
		float mixed_sample = 0.0f;

		for(int j = 0; j < MAX_VOICES; ++j){
			if(voices[j].active){
				float phase = voices[j].phase;
				float sample_val = 0.0f;

				switch(current_wave_type){
				 case SIN_WAVE:
					 sample_val += sinf(2.0f * M_PI * phase);
					 break;
				 case SQUARE_WAVE:
					 sample_val = (phase < 0.5f) ? 1.0f : -1.0f;
					 break;
				 case SAWTOOTH_WAVE:
					 sample_val = (2.0f * phase) - 1.0f;
					 break;
				 case TRIANGLE_WAVE:
					 if(phase < 0.5f){
						 sample_val = 4.0f*phase - 1.0f;
					 }
					 else{
						 sample_val = -4.0f*phase + 3.0f;
					 }
					 break;
				}

				mixed_sample += sample_val;
				voices[j].phase += voices[j].phase_step;

				if(voices[j].phase >= 1.0f){
					voices[j].phase -= 1.0f;
				}
			}
		}
		mixed_sample = (mixed_sample / MAX_VOICES) * MAX_AMPLITUDE;
		buffer[i] = 2048 + (uint16_t)(mixed_sample);
	}
}

//void HAL_DAC_ConvHalfCpltCallback(DAC_HandleTypeDef *hdac)
//{
//    if (hdac->Instance == DAC)
//    {
//        HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
//
//        static uint32_t half_count = 0;
//        half_count++;
//        if(half_count % 1000 == 0) {
//            printf("Half: %lu\r\n", half_count);
//        }
//
//        fill_audio_buffer(&audio_buffer[0], AUDIO_BUFFER_SIZE / 2);
//    }
//}
//
//void HAL_DAC_ConvCpltCallback(DAC_HandleTypeDef *hdac)
//{
//    if (hdac->Instance == DAC)
//    {
//        static uint32_t full_count = 0;
//        full_count++;
//        if(full_count % 1000 == 0) {
//            printf("Full: %lu\r\n", full_count);
//        }
//
//        fill_audio_buffer(&audio_buffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2);
//    }
//}

void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);

    fill_audio_buffer(&audio_buffer[0], AUDIO_BUFFER_SIZE / 2);
}

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    fill_audio_buffer(&audio_buffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2);
}

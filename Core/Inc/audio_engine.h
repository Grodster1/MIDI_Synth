/*
 * audio_engine.h
 *
 *  Created on: Oct 14, 2025
 *      Author: wiku2
 */

#ifndef INC_AUDIO_ENGINE_H_
#define INC_AUDIO_ENGINE_H_

#include <stdint.h>


typedef enum{
	SIN_WAVE,
	SQUARE_WAVE,
	SAWTOOTH_WAVE,
	TRIANGLE_WAVE
} WaveType;

void AudioEngine_Init(void);
void AudioEngine_PlayTestTone(void);
void fill_audio_buffer(uint16_t* buffer, uint16_t size);
void AudioEngine_PlayNote(uint8_t note, float frequency);
void AudioEngine_StopNote(uint8_t note);
float AudioEngine_MIDINoteToFrequency(uint8_t note);
void AudioEngine_ProcessBuffer(void);
void AudioEngine_SetWaveType(WaveType type);
#endif /* INC_AUDIO_ENGINE_H_ */

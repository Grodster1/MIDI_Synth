/*
 * audio_engine.h
 *
 *  Created on: Oct 14, 2025
 *      Author: wiku2
 */

#ifndef INC_AUDIO_ENGINE_H_
#define INC_AUDIO_ENGINE_H_

#include <stdint.h>

void AudioEngine_Init(void);
void AudioEngine_PlayTestTone(void);
void AudioEngine_Stop(void);
void fill_audio_buffer(uint16_t* buffer, uint16_t size);
void AudioEngine_PlayNote(float frequency);
void AudioEngine_ProcessBuffer(void);
#endif /* INC_AUDIO_ENGINE_H_ */

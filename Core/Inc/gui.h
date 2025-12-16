#ifndef GUI_H
#define GUI_H

/* Includes */
#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"

/* Waveforms Waveform Types */
#define WAVEFORM_SINE     0
#define WAVEFORM_SQUARE   1
#define WAVEFORM_SAW      2
#define WAVEFORM_TRIANGLE 3

/* Structure */
typedef struct {
    uint8_t current_octave;
    uint8_t current_waveform;
    uint8_t last_note;
} SynthState;


/**
  * @brief  Initializes the GUI module, including LCD and Touch Screen.
  * Also applies necessary hardware workarounds for the I2C bus.
  * @param  None
  * @retval None
  */
void GUI_Init(void);

/**
  * @brief  Draws the entire interface (buttons, labels, piano keys) on the LCD.
  * @param  None
  * @retval None
  */
void GUI_DrawInterface(void);

/**
  * @brief  Handles touch screen interactions.
  * Detects button presses and piano key hits, updates the state,
  * and triggers audio events.
  * @param  None
  * @retval None
  */
void GUI_HandleTouch(void);


#endif /* GUI_H */

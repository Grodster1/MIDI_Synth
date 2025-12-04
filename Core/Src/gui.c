/* Includes */
#include "gui.h"
#include <stdio.h>

/* External function references */
extern void I2Cx_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);

/* Private defines */
#define STMPE811_ADDR   0x82

/* UI Layout Constants  */
#define Y_WAVE_BTNS     20
#define Y_OCTAVE_BTNS   80
#define Y_PIANO_KEYS    150
#define H_PIANO_KEYS    170

/* Global variables */
/**
  * @brief Global synthesizer state instance.
  * Initialized to Octave 4, Waveform 0 (Sine).
  */
SynthState synth = {4, 0, 0};


/**
  * @brief  Initializes the LCD and Touch Screen hardware.
  * This function performs the specific startup sequence required
  * for the STM32F429I-Discovery board, including a workaround
  * to wake up the STMPE811 controller.
  * @param  None
  * @retval None
  */
void GUI_Init(void) {
    /* Initialize LCD */
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_DisplayOn();

    /* Set default screen properties */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);

    /* Initialize Touch Screen Driver */
    HAL_Delay(100);
    BSP_TS_Init(240, 320);

    /* STMPE811 Hardware Fix */
    I2Cx_Write(STMPE811_ADDR, 0x04, 0x00); // SYS_CTRL2: Enable Clocks
    HAL_Delay(10);
    I2Cx_Write(STMPE811_ADDR, 0x40, 0x01); // TSC_CTRL: Enable Touch Screen
    HAL_Delay(10);
    I2Cx_Write(STMPE811_ADDR, 0x4A, 0x01); // FIFO_STA: Reset FIFO
    I2Cx_Write(STMPE811_ADDR, 0x4A, 0x00); // FIFO_STA: Enable FIFO

    /* Draw the initial UI */
    GUI_DrawInterface();
}

/**
  * @brief  Redraws the graphical user interface elements.
  * Updates buttons colors based on the current state.
  * @param  None
  * @retval None
  */
void GUI_DrawInterface(void) {

    char buf[20];
    const char* waves[] = {"SIN", "SQU", "SAW", "TRI"};

    for(int i = 0; i < 4; i++) {
        /* Highlight the selected waveform */
        if(synth.current_waveform == i) {
            BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
        } else {
            BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
        }

        /* Draw button rectangle */
        BSP_LCD_FillRect(10 + (i * 55), Y_WAVE_BTNS, 50, 40);

        /* Draw button text */
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_SetBackColor(synth.current_waveform == i ? LCD_COLOR_GREEN : LCD_COLOR_LIGHTGRAY);
        BSP_LCD_DisplayStringAt(20 + (i * 55), Y_WAVE_BTNS + 12, (uint8_t*)waves[i], LEFT_MODE);
    }

    /* Reset background color */
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    /* Draw '-' and '+' buttons */
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_FillRect(10, Y_OCTAVE_BTNS, 60, 40);  // Minus button
    BSP_LCD_FillRect(170, Y_OCTAVE_BTNS, 60, 40); // Plus button

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_DisplayStringAt(30, Y_OCTAVE_BTNS + 10, (uint8_t*)"-", LEFT_MODE);
    BSP_LCD_DisplayStringAt(190, Y_OCTAVE_BTNS + 10, (uint8_t*)"+", LEFT_MODE);

    /* Display current octave number */
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    sprintf(buf, "OCTAVE: %d", synth.current_octave);
    BSP_LCD_DisplayStringAt(0, Y_OCTAVE_BTNS + 12, (uint8_t*)buf, CENTER_MODE);

    /* Draw Piano Keys */
    const char* notes[] = {"C", "D", "E", "F", "G", "A", "H"};
    int key_width = 34;

    for(int i = 0; i < 7; i++) {
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        /* Draw key outline */
        BSP_LCD_DrawRect(1 + (i * key_width), Y_PIANO_KEYS, key_width, H_PIANO_KEYS);
        /* Draw key label */
        BSP_LCD_DisplayStringAt(10 + (i * key_width), Y_PIANO_KEYS + 140, (uint8_t*)notes[i], LEFT_MODE);
    }
}

/**
  * @brief  Polls the Touch Screen state and executes actions based on touch coordinates.
  * Manages button presses (Waveform/Octave) and piano key events.
  * @param  None
  * @retval None
  */
void GUI_HandleTouch(void) {

    TS_StateTypeDef TS_State;

    /*  Get current touch state */
    BSP_TS_GetState(&TS_State);

    if (TS_State.TouchDetected) {
        uint16_t x = TS_State.X;
        uint16_t y = TS_State.Y;

        /* Waveforms */
        if (y >= Y_WAVE_BTNS && y <= Y_WAVE_BTNS + 40) {
            for(int i = 0; i < 4; i++) {
                /* Check X bounds for each button */
                if(x >= 10 + (i * 55) && x <= 60 + (i * 55)) {
                    synth.current_waveform = i;

                    /* Audio Engine func */

                    /* Refresh UI to show selection */
                    GUI_DrawInterface();

                    /* Debounce delay */
                    HAL_Delay(150);
                    return;
                }
            }
        }

        /* Octave */
        if (y >= Y_OCTAVE_BTNS && y <= Y_OCTAVE_BTNS + 40) {
            /* Check Minus Button */
            if (x >= 10 && x <= 70) {
                if(synth.current_octave > 1) synth.current_octave--;
            }
            /* Check Plus Button */
            else if (x >= 170 && x <= 230) {
                if(synth.current_octave < 7) synth.current_octave++;
            }

            GUI_DrawInterface();
            HAL_Delay(150);
            return;
        }

        /* Piano Keys */
        if (y >= Y_PIANO_KEYS) {
            int key_width = 34;
            int key_index = x / key_width; // Calculates index 0..6

            if (key_index >= 0 && key_index <= 6) {
                /* Adding semitones */
                int semitones[] = {0, 2, 4, 5, 7, 9, 11};

                /* Calculate MIDI Note Number */
                uint8_t midi_note = ((synth.current_octave + 1) * 12) + semitones[key_index];

                /* Calculate frequency and play note */
                float freq = MIDINoteToFrequency(midi_note);
                /* Audio Enigne func */

                /* Highlight pressed key */
                BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
                BSP_LCD_FillRect(1 + (key_index * key_width), Y_PIANO_KEYS, key_width, H_PIANO_KEYS);

                /* Hold the note */
                do {
                    BSP_TS_GetState(&TS_State);
                    HAL_Delay(5);
                } while(TS_State.TouchDetected);

                /* Stop the note */
                /* Audio Engine func */

                /* Remove visual highlight */
                BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
                BSP_LCD_FillRect(1 + (key_index * key_width) + 1, Y_PIANO_KEYS + 1, key_width - 2, H_PIANO_KEYS - 2);

                /* Restore grid */
                GUI_DrawInterface();
            }
        }
    }
}


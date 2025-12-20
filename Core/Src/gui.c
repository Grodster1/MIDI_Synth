/* Includes */
#include "gui.h"
#include "audio_engine.h"
#include <stdio.h>

/* External function references */
extern void I2Cx_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);

/* Private defines */
#define STMPE811_ADDR   0x82

/* UI Layout Constants  */
#define Y_WAVE_BTNS     20
#define Y_OCTAVE_BTNS   80

/* Piano Layout Constants */
#define Y_PIANO_KEYS    150
#define W_WHITE         34      // Szerokość białego klawisza
#define H_WHITE         170     // Wysokość białego klawisza
#define W_BLACK         20      // Szerokość czarnego klawisza
#define H_BLACK         100     // Wysokość czarnego klawisza

/* Global variables */
SynthState synth = {4, 0, 0};

/* Note labels (Global so HandleTouch can access them for redraw) */
const char* notes[] = {"C", "D", "E", "F", "G", "A", "H"};

/**
  * @brief  Initializes the LCD and Touch Screen hardware.
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
  */
void GUI_DrawInterface(void) {
    char buf[20];
    const char* waves[] = {"SIN", "SQU", "SAW", "TRI"};

    /* --- Waveform Buttons --- */
    for(int i = 0; i < 4; i++) {
        if(synth.current_waveform == i) {
            BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
        } else {
            BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
        }
        BSP_LCD_FillRect(10 + (i * 55), Y_WAVE_BTNS, 50, 40);

        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_SetBackColor(synth.current_waveform == i ? LCD_COLOR_GREEN : LCD_COLOR_LIGHTGRAY);
        BSP_LCD_DisplayStringAt(20 + (i * 55), Y_WAVE_BTNS + 12, (uint8_t*)waves[i], LEFT_MODE);
    }
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    /* --- Octave Buttons --- */
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_FillRect(10, Y_OCTAVE_BTNS, 60, 40);
    BSP_LCD_FillRect(170, Y_OCTAVE_BTNS, 60, 40);

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_DisplayStringAt(30, Y_OCTAVE_BTNS + 10, (uint8_t*)"-", LEFT_MODE);
    BSP_LCD_DisplayStringAt(190, Y_OCTAVE_BTNS + 10, (uint8_t*)"+", LEFT_MODE);

    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    sprintf(buf, "OCTAVE: %d", synth.current_octave);
    BSP_LCD_DisplayStringAt(0, Y_OCTAVE_BTNS + 12, (uint8_t*)buf, CENTER_MODE);

    /* --- Piano Keys --- */

    /* White Keys */
    for(int i = 0; i < 7; i++) {
        // Clear
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillRect(1 + (i * W_WHITE), Y_PIANO_KEYS, W_WHITE, H_WHITE);

        // Draw
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(1 + (i * W_WHITE), Y_PIANO_KEYS, W_WHITE, H_WHITE);
        BSP_LCD_DisplayStringAt(10 + (i * W_WHITE), Y_PIANO_KEYS + 140, (uint8_t*)notes[i], LEFT_MODE);
    }

    /* Black Keys */
    int has_black[] = {1, 1, 0, 1, 1, 1, 0}; // C#, D#, -, F#, G#, A#, -

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

    for(int i = 0; i < 7; i++) {
        if(has_black[i]) {
            int x_pos = 1 + ((i + 1) * W_WHITE) - (W_BLACK / 2);
            BSP_LCD_FillRect(x_pos, Y_PIANO_KEYS, W_BLACK, H_BLACK);
        }
    }
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
}

/**
  * @brief  Polls the Touch Screen state.
  */
void GUI_HandleTouch(void) {
    TS_StateTypeDef TS_State;
    BSP_TS_GetState(&TS_State);

    if (TS_State.TouchDetected) {
        uint16_t x = TS_State.X;
        uint16_t y = TS_State.Y;

        /* --- Waveforms --- */
        if (y >= Y_WAVE_BTNS && y <= Y_WAVE_BTNS + 40) {
            for(int i = 0; i < 4; i++) {
                if(x >= 10 + (i * 55) && x <= 60 + (i * 55)) {
                	synth.current_waveform = i;
                	AudioEngine_SetWaveType((WaveType)i);
                    GUI_DrawInterface();
                    HAL_Delay(150);
                    return;
                }
            }
        }

        /* --- Octave --- */
        if (y >= Y_OCTAVE_BTNS && y <= Y_OCTAVE_BTNS + 40) {
            if (x >= 10 && x <= 70) {
                if(synth.current_octave > 1) synth.current_octave--;
            }
            else if (x >= 170 && x <= 230) {
                if(synth.current_octave < 7) synth.current_octave++;
            }
            GUI_DrawInterface();

            /* Wait for release */
            do {
                BSP_TS_GetState(&TS_State);
                HAL_Delay(10);
            } while(TS_State.TouchDetected);

            return;
        }

        /* --- Piano Keys Logic --- */
        if (y >= Y_PIANO_KEYS) {

            int note_semitone = -1;
            int is_black = 0;
            int draw_idx = -1;

            int has_black[] = {1, 1, 0, 1, 1, 1, 0};
            int black_vals[] = {1, 3, -1, 6, 8, 10, -1};
            int white_vals[] = {0, 2, 4, 5, 7, 9, 11};

            /* Black Keys */
            if (y <= Y_PIANO_KEYS + H_BLACK) {
                for (int i = 0; i < 6; i++) {
                    if (has_black[i]) {
                        int center_x = 1 + ((i + 1) * W_WHITE);
                        if (x >= (center_x - W_BLACK/2) && x <= (center_x + W_BLACK/2)) {
                            note_semitone = black_vals[i];
                            draw_idx = i;
                            is_black = 1;
                            break;
                        }
                    }
                }
            }

            /* White Keys */
            if (note_semitone == -1) {
                int white_idx = x / W_WHITE;
                if (white_idx >= 0 && white_idx <= 6) {
                    note_semitone = white_vals[white_idx];
                    draw_idx = white_idx;
                    is_black = 0;
                }
            }

            /* Execute */
            if (note_semitone != -1) {

                uint8_t midi_note = ((synth.current_octave + 1) * 12) + note_semitone;

                /* --- AUDIO ON FUNC HERE --- */
                float freq = AudioEngine_MIDINoteToFrequency(midi_note);
                AudioEngine_PlayNote(midi_note, freq);
                /* Highlight */
                if (is_black) {
                    BSP_LCD_SetTextColor(LCD_COLOR_RED);
                    int x_pos = 1 + ((draw_idx + 1) * W_WHITE) - (W_BLACK / 2);
                    BSP_LCD_FillRect(x_pos, Y_PIANO_KEYS, W_BLACK, H_BLACK);
                }
                else {
                    // White Key pressed
                    BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
                    BSP_LCD_FillRect(1 + (draw_idx * W_WHITE), Y_PIANO_KEYS, W_WHITE, H_WHITE);

                    // Restore Black Keys
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                    if (draw_idx > 0 && has_black[draw_idx - 1]) {
                         int x_left = 1 + (draw_idx * W_WHITE) - (W_BLACK / 2);
                         BSP_LCD_FillRect(x_left, Y_PIANO_KEYS, W_BLACK, H_BLACK);
                    }
                    if (draw_idx < 6 && has_black[draw_idx]) {
                         int x_right = 1 + ((draw_idx + 1) * W_WHITE) - (W_BLACK / 2);
                         BSP_LCD_FillRect(x_right, Y_PIANO_KEYS, W_BLACK, H_BLACK);
                    }
                }

                /* Wait for release */
                do {
                    BSP_TS_GetState(&TS_State);
                    HAL_Delay(5);
                } while(TS_State.TouchDetected);

                /* --- AUDIO OFF FUNC HERE --- */
                AudioEngine_StopNote(midi_note);
                /* Restore */
                if (is_black) {
                    // Black Keys
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                    int x_pos = 1 + ((draw_idx + 1) * W_WHITE) - (W_BLACK / 2);
                    BSP_LCD_FillRect(x_pos, Y_PIANO_KEYS, W_BLACK, H_BLACK);
                }
                else {
                    // White Keys

                	// Clear
                    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
                    BSP_LCD_FillRect(1 + (draw_idx * W_WHITE), Y_PIANO_KEYS, W_WHITE, H_WHITE);

                    // Draw
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                    BSP_LCD_DrawRect(1 + (draw_idx * W_WHITE), Y_PIANO_KEYS, W_WHITE, H_WHITE);

                    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
                    BSP_LCD_DisplayStringAt(10 + (draw_idx * W_WHITE), Y_PIANO_KEYS + 140, (uint8_t*)notes[draw_idx], LEFT_MODE);

                    // Redraw Black
                    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

                    if (draw_idx > 0 && has_black[draw_idx - 1]) {
                         int x_left = 1 + (draw_idx * W_WHITE) - (W_BLACK / 2);
                         BSP_LCD_FillRect(x_left, Y_PIANO_KEYS, W_BLACK, H_BLACK);
                    }
                    if (draw_idx < 6 && has_black[draw_idx]) {
                         int x_right = 1 + ((draw_idx + 1) * W_WHITE) - (W_BLACK / 2);
                         BSP_LCD_FillRect(x_right, Y_PIANO_KEYS, W_BLACK, H_BLACK);
                    }
                    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
                }
            }
        }
    }
}

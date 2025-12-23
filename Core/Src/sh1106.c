/*
 * sh1106.c
 *
 *  Created on: Dec 23, 2025
 *      Author: wiktor
 */



#include "sh1106.h"
#include "main.h"
#include "../../Utilities/Fonts/fonts.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c3;
static uint8_t SH1106_Buffer[1024];

void SH1106_WriteCommand(uint8_t command) {
    HAL_I2C_Mem_Write(&hi2c3, SH1106_ADDR, 0x00, 1, &command, 1, 10);
}

void SH1106_WriteData(uint8_t* data, uint16_t size) {
    HAL_I2C_Mem_Write(&hi2c3, SH1106_ADDR, 0x40, 1, data, size, 100);
}

void SH1106_Init(void) {
    HAL_Delay(100);

    SH1106_WriteCommand(0xAE); // Display OFF
    SH1106_WriteCommand(0xD5); // Set Display Clock Divide Ratio
    SH1106_WriteCommand(0x80);
    SH1106_WriteCommand(0xA8); // Set Multiplex Ratio
    SH1106_WriteCommand(0x3F);
    SH1106_WriteCommand(0xD3); // Set Display Offset
    SH1106_WriteCommand(0x00);
    SH1106_WriteCommand(0x40); // Set Display Start Line
    SH1106_WriteCommand(0x8D); // Charge Pump Setting
    SH1106_WriteCommand(0x14); // Enable Charge Pump
    SH1106_WriteCommand(0xA1); // Segment Re-map (Mirror X)
    SH1106_WriteCommand(0xC8); // COM Output Scan Direction (Mirror Y)
    SH1106_WriteCommand(0xDA); // Set COM Pins Hardware Configuration
    SH1106_WriteCommand(0x12);
    SH1106_WriteCommand(0x81); // Set Contrast Control
    SH1106_WriteCommand(0xCF);
    SH1106_WriteCommand(0xD9); // Set Pre-charge Period
    SH1106_WriteCommand(0xF1);
    SH1106_WriteCommand(0xDB); // Set VCOMH Deselect Level
    SH1106_WriteCommand(0x40);
    SH1106_WriteCommand(0xA4); // Entire Display ON (Resume)
    SH1106_WriteCommand(0xA6); // Normal Display
    SH1106_WriteCommand(0xAF); // Display ON

    SH1106_Fill(0);
    SH1106_UpdateScreen();
}

void SH1106_Fill(uint8_t color) {
    uint8_t fill = (color == 0) ? 0x00 : 0xFF;
    for(int i = 0; i < sizeof(SH1106_Buffer); i++) {
        SH1106_Buffer[i] = fill;
    }
}

void SH1106_UpdateScreen(void) {
    for(uint8_t i = 0; i < 8; i++) {
        SH1106_WriteCommand(0xB0 + i);

        SH1106_WriteCommand(0x02);
        SH1106_WriteCommand(0x10);

        SH1106_WriteData(&SH1106_Buffer[128 * i], 128);
    }
}

void SH1106_DrawPixel(uint16_t x, uint16_t y, uint8_t color) {
    if(x >= 128 || y >= 64) return;

    if(color) {
        SH1106_Buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    } else {
        SH1106_Buffer[x + (y / 8) * 128] &= ~(1 << (y % 8));
    }
}


void SH1106_DrawChar(uint16_t x, uint16_t y, char pChar, sFONT* pFont, uint8_t color) {
    uint32_t i = 0, j = 0;
    uint16_t height, width;
    uint8_t offset;
    uint8_t *pCharTable;
    uint8_t *pFontTable = (uint8_t *)pFont->table;

    height = pFont->Height;
    width = pFont->Width;

    offset = 8 * ((width + 7) / 8) - width;
    pCharTable = pFontTable + ((pChar - ' ') * height * ((width + 7) / 8));

    for (i = 0; i < height; i++) {
        uint8_t *prow = pCharTable + (i * ((width + 7) / 8));

        for (j = 0; j < width; j++) {
            uint8_t currentByte = prow[j / 8];
            if ((currentByte << (j % 8)) & 0x80) {
                SH1106_DrawPixel(x + j, y + i, color);
            } else {
                SH1106_DrawPixel(x + j, y + i, !color);
            }
        }
    }
}

void SH1106_DrawString(uint16_t x, uint16_t y, char* pString, sFONT* pFont, uint8_t color) {
    while (*pString != '\0') {
        if ((x + pFont->Width) > 128) {
            x = 0;
            y += pFont->Height;
        }

        SH1106_DrawChar(x, y, *pString, pFont, color);
        x += pFont->Width;
        pString++;
    }
}

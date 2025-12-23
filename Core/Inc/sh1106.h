/*
 * sh1106.h
 *
 *  Created on: Dec 23, 2025
 *      Author: wiktor
 */

#ifndef INC_SH1106_H_
#define INC_SH1106_H_

#include "stm32f4xx_hal.h"
#include "../../Utilities/Fonts/fonts.h"

#define SH1106_ADDR 0x78

void SH1106_Init(void);
void SH1106_Fill(uint8_t color);
void SH1106_UpdateScreen(void);
void SH1106_DrawPixel(uint16_t x, uint16_t y, uint8_t color);

void SH1106_DrawChar(uint16_t x, uint16_t y, char pChar, sFONT* pFont, uint8_t color);
void SH1106_DrawString(uint16_t x, uint16_t y, char* pString, sFONT* pFont, uint8_t color);


#endif /* INC_SH1106_H_ */

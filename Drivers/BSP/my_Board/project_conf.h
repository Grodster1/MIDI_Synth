/* project_conf.h */
#ifndef PROJECT_CONF_H
#define PROJECT_CONF_H

#include "stm32f4xx_hal.h"
#include "main.h"

/* TypeDef */
#define LCD_LayerCfgTypeDef  LTDC_LayerCfgTypeDef

/* Handles */
extern LTDC_HandleTypeDef hltdc;
extern DMA2D_HandleTypeDef hdma2d;
extern SDRAM_HandleTypeDef hsdram1;
extern I2C_HandleTypeDef hi2c1;

/* Defines */
#define hsdram_eval         hsdram1
#define hltdc_eval          hltdc
#define hdma2d_eval         hdma2d
#define hts_eval            hi2c1

#endif

/* project_conf.h */
#ifndef PROJECT_CONF_H
#define PROJECT_CONF_H

#include "stm32f4xx_hal.h"  // Zmień na swoją serię (f4, f7, h7)
#include "main.h"           // Tu są definicje pinów z CubeMX

/* Definicje wymagane przez BSP LCD */
#define LCD_LayerCfgTypeDef  LTDC_LayerCfgTypeDef

/* Mapowanie uchwytów (Handles) z main.c do nazw używanych w BSP */
/* Sprawdź w main.c jak nazywają się twoje uchwyty! */
extern LTDC_HandleTypeDef hltdc;
extern DMA2D_HandleTypeDef hdma2d;
extern SDRAM_HandleTypeDef hsdram1;
extern I2C_HandleTypeDef hi2c1;

/* Te nazwy (po lewej) są używane w plikach BSP ST */
#define hsdram_eval         hsdram1
#define hltdc_eval          hltdc
#define hdma2d_eval         hdma2d
#define hts_eval            hi2c1

#endif

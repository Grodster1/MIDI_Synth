/**
  ******************************************************************************
  * @file    stm32f429i_discovery_ts.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage Touch
  *          screen available with STMPE811 IO Expander device mounted on
  *          STM32F429I-Discovery Kit.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "project_conf.h"
#include "stm32f429i_discovery_ts.h"
#include "stm32f429i_discovery_io.h"
#include "../Components/stmpe811/stmpe811.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32F429I_DISCOVERY
  * @{
  */

/** @defgroup STM32F429I_DISCOVERY_TS STM32F429I DISCOVERY TS
  * @{
  */

/** @defgroup STM32F429I_DISCOVERY_TS_Private_Types_Definitions STM32F429I DISCOVERY TS Private Types Definitions
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_TS_Private_Defines STM32F429I DISCOVERY TS Private Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_TS_Private_Macros STM32F429I DISCOVERY TS Private Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_TS_Private_Variables STM32F429I DISCOVERY TS Private Variables
  * @{
  */
static TS_DrvTypeDef     *TsDrv;
static uint16_t          TsXBoundary, TsYBoundary;
/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_TS_Private_Function_Prototypes STM32F429I DISCOVERY TS Private Function Prototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_TS_Private_Functions STM32F429I DISCOVERY TS Private Functions
  * @{
  */

/**
  * @brief  Initializes and configures the touch screen functionalities and
  *         configures all necessary hardware resources (GPIOs, clocks..).
  * @param  XSize: The maximum X size of the TS area on LCD
  * @param  YSize: The maximum Y size of the TS area on LCD
  * @retval TS_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_Init(uint16_t XSize, uint16_t YSize)
{
  uint8_t ret = TS_ERROR;

  /* Initialize x and y positions boundaries */
  TsXBoundary = XSize;
  TsYBoundary = YSize;

  /* Read ID and verify if the IO expander is ready */
  if (stmpe811_ts_drv.ReadID(TS_I2C_ADDRESS) == STMPE811_ID)
  {
    /* Initialize the TS driver structure */
    TsDrv = &stmpe811_ts_drv;

    ret = TS_OK;
  }

  if (ret == TS_OK)
  {
    /* Initialize the LL TS Driver */
    TsDrv->Init(TS_I2C_ADDRESS);
    TsDrv->Start(TS_I2C_ADDRESS);
  }

  return ret;
}

/**
  * @brief  Configures and enables the touch screen interrupts.
  * @retval TS_OK: if ITconfig is OK. Other value if error.
  */
uint8_t BSP_TS_ITConfig(void)
{
  /* Enable the TS ITs */
  TsDrv->EnableIT(TS_I2C_ADDRESS);

  return TS_OK;
}

/**
  * @brief  Gets the TS IT status.
  * @retval Interrupt status.
  */
uint8_t BSP_TS_ITGetStatus(void)
{
  /* Return the TS IT status */
  return (TsDrv->GetITStatus(TS_I2C_ADDRESS));
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TsState: Pointer to touch screen current state structure
  */
void BSP_TS_GetState(TS_StateTypeDef *TsState)
{
  uint16_t x_raw, y_raw;

  /* Chech if touched  */
  TsState->TouchDetected = TsDrv->DetectTouch(TS_I2C_ADDRESS);

  if (TsState->TouchDetected)
  {
    /* Get raw data */
    TsDrv->GetXY(TS_I2C_ADDRESS, &x_raw, &y_raw);

    /* Convert data */
    TsState->X = (uint16_t)((uint32_t)x_raw * 240 / 4096);
    TsState->Y = (uint16_t)((uint32_t)y_raw * 320 / 4096);
  }
  else
  {
	/* No touch */
    TsState->X = 0;
    TsState->Y = 0;
  }
}

/**
  * @brief  Clears all touch screen interrupts.
  */
void BSP_TS_ITClear(void)
{
  /* Clear TS IT pending bits */
  TsDrv->ClearIT(TS_I2C_ADDRESS);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/** Drivers/BSP/Components/Common/io.h (Legacy Version) **/
#ifndef __IO_H
#define __IO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup IO
  * @{
  */

/** @defgroup IO_Exported_Types
  * @{
  */

typedef enum
{
  IO_MODE_INPUT = 0,   /* Input Floating Mode */
  IO_MODE_OUTPUT,      /* Output Push Pull Mode */
  IO_MODE_IT_RISING_EDGE,   /* Float input - Irq detect on Rising edge */
  IO_MODE_IT_FALLING_EDGE,  /* Float input - Irq detect on Falling edge */
  IO_MODE_IT_LOW_LEVEL,     /* Float input - Irq detect on Low level */
  IO_MODE_IT_HIGH_LEVEL,    /* Float input - Irq detect on High level */
  /* Tryby specyficzne dla STMPE811 */
  IO_MODE_OFF,
  IO_MODE_ANALOG,
  IO_MODE_INPUT_PU,
  IO_MODE_INPUT_PD,
  IO_MODE_OUTPUT_OD,
  IO_MODE_OUTPUT_PP_PU,
  IO_MODE_OUTPUT_PP_PD
} IO_ModeTypedef;

/** @defgroup IO_Driver_structure  IO Driver structure
  * @{
  */
typedef struct
{
  void       (*Init)(uint16_t);
  uint16_t   (*ReadID)(uint16_t);
  void       (*Reset)(uint16_t);
  void       (*Start)(uint16_t, uint32_t);
  uint8_t    (*Config)(uint16_t, uint32_t, IO_ModeTypedef);
  void       (*WritePin)(uint16_t, uint32_t, uint8_t);
  uint32_t   (*ReadPin)(uint16_t, uint32_t);
  void       (*EnableIT)(uint16_t);
  void       (*DisableIT)(uint16_t);
  uint32_t   (*ITStatus)(uint16_t, uint32_t);
  void       (*ClearIT)(uint16_t, uint32_t);
} IO_DrvTypeDef;
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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __IO_H */

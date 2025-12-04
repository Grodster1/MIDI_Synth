#ifndef __STMPE811_H
#define __STMPE811_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../Common/ts.h"
#include "../Common/io.h"

/* --- Definicje Rejestrów (Poprawione i scalone) --- */
#define STMPE811_ID                     0x0811

/* Identification registers & System Control */
#define STMPE811_CHP_ID_LSB_REG         0x01
#define STMPE811_CHP_ID_MSB_REG         0x00
#define STMPE811_ID_VER_REG             0x02

/* General Control Registers */
#define STMPE811_SYS_CTRL1_REG          0x03
#define STMPE811_SYS_CTRL2_REG          0x04
#define STMPE811_SPI_CFG_REG            0x08

/* Interrupt system Registers */
#define STMPE811_INT_CTRL_REG           0x09
#define STMPE811_INT_EN_REG             0x0A
#define STMPE811_INT_STA_REG            0x0B
#define STMPE811_IO_INT_EN_REG          0x0C
#define STMPE811_IO_INT_STA_REG         0x0D

/* IO Registers */
#define STMPE811_IO_SET_PIN_REG         0x10
#define STMPE811_IO_CLR_PIN_REG         0x11
#define STMPE811_IO_MP_STA_REG          0x12
#define STMPE811_IO_DIR_REG             0x13
#define STMPE811_IO_ED_REG              0x14
#define STMPE811_IO_RE_REG              0x15
#define STMPE811_IO_FE_REG              0x16
#define STMPE811_IO_AF_REG              0x17

/* ADC Registers */
#define STMPE811_ADC_INT_EN_REG         0x0E
#define STMPE811_ADC_INT_STA_REG        0x0F
#define STMPE811_ADC_CTRL1_REG          0x20
#define STMPE811_ADC_CTRL2_REG          0x21
#define STMPE811_ADC_CAPT_REG           0x22
#define STMPE811_ADC_DATA_CH0_REG       0x30
#define STMPE811_ADC_DATA_CH1_REG       0x32
#define STMPE811_ADC_DATA_CH2_REG       0x34
#define STMPE811_ADC_DATA_CH3_REG       0x36
#define STMPE811_ADC_DATA_CH4_REG       0x38
#define STMPE811_ADC_DATA_CH5_REG       0x3A
#define STMPE811_ADC_DATA_CH6_REG       0x3B
#define STMPE811_ADC_DATA_CH7_REG       0x3C

/* Touch Screen Registers */
#define STMPE811_TSC_CTRL_REG           0x40
#define STMPE811_TSC_CFG_REG            0x41
#define STMPE811_WDW_TR_X_REG           0x42
#define STMPE811_WDW_TR_Y_REG           0x44
#define STMPE811_WDW_BL_X_REG           0x46
#define STMPE811_WDW_BL_Y_REG           0x48
#define STMPE811_FIFO_TH_REG            0x4A
#define STMPE811_FIFO_STA_REG           0x4B
#define STMPE811_FIFO_SIZE_REG          0x4C
#define STMPE811_TSC_DATA_X_REG         0x4D
#define STMPE811_TSC_DATA_Y_REG         0x4F
#define STMPE811_TSC_DATA_Z_REG         0x51
#define STMPE811_TSC_DATA_XYZ_REG       0x52
#define STMPE811_TSC_FRACT_XYZ_REG      0x56
#define STMPE811_TSC_DATA_INC_REG       0x57
#define STMPE811_TSC_DATA_NON_INC_REG   0xD7
#define STMPE811_TSC_I_DRIVE_REG        0x58
#define STMPE811_TSC_SHIELD_REG         0x59

/* --- Exported functions (Legacy API) --- */
void     stmpe811_Init(uint16_t DeviceAddr);
void     stmpe811_Reset(uint16_t DeviceAddr);
uint16_t stmpe811_ReadID(uint16_t DeviceAddr);
void     stmpe811_TS_Start(uint16_t DeviceAddr);
uint8_t  stmpe811_TS_DetectTouch(uint16_t DeviceAddr);
void     stmpe811_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);
void     stmpe811_TS_EnableIT(uint16_t DeviceAddr);
void     stmpe811_TS_DisableIT(uint16_t DeviceAddr);
uint8_t  stmpe811_TS_ITStatus(uint16_t DeviceAddr);
void     stmpe811_TS_ClearIT(uint16_t DeviceAddr);

void     stmpe811_IO_Start(uint16_t DeviceAddr, uint32_t IO_Pin);
uint8_t  stmpe811_IO_Config(uint16_t DeviceAddr, uint32_t IO_Pin, IO_ModeTypedef IO_Mode);
void     stmpe811_IO_WritePin(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t PinState);
uint32_t stmpe811_IO_ReadPin(uint16_t DeviceAddr, uint32_t IO_Pin);
void     stmpe811_IO_EnableIT(uint16_t DeviceAddr);
void     stmpe811_IO_DisableIT(uint16_t DeviceAddr);
uint32_t stmpe811_IO_ITStatus(uint16_t DeviceAddr, uint32_t IO_Pin);
void     stmpe811_IO_ClearIT(uint16_t DeviceAddr, uint32_t IO_Pin);

/* Struktury wymagane przez stm32f429i_discovery_ts.c */
extern TS_DrvTypeDef stmpe811_ts_drv;
extern IO_DrvTypeDef stmpe811_io_drv;

#ifdef __cplusplus
}
#endif
#endif /* __STMPE811_H */

#include "stmpe811.h"

/* IMPORT FUNKCJI I2C Z PLIKU MAIN.C (lub zdefiniowanych w BSP) */
/* Funkcje te muszą być zdefiniowane jako extern, żeby linker je znalazł */
extern void     I2Cx_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t  I2Cx_Read(uint8_t Addr, uint8_t Reg);
extern void     I2Cx_Error(uint8_t Addr);

/* Helpery */
void stmpe811_WriteReg(uint16_t DeviceAddr, uint8_t Reg, uint8_t Value)
{
  I2Cx_Write((uint8_t)DeviceAddr, Reg, Value);
}

uint8_t stmpe811_ReadReg(uint16_t DeviceAddr, uint8_t Reg)
{
  return I2Cx_Read((uint8_t)DeviceAddr, Reg);
}

/* --- Touch Screen Functions --- */

void stmpe811_Init(uint16_t DeviceAddr)
{
  stmpe811_Reset(DeviceAddr);
}

void stmpe811_Reset(uint16_t DeviceAddr)
{
  /* SYS_CTRL1: SOFT_RESET */
  stmpe811_WriteReg(DeviceAddr, STMPE811_SYS_CTRL1_REG, 0x02);
  HAL_Delay(10);
  /* Reset bitu resetu */
  stmpe811_WriteReg(DeviceAddr, STMPE811_SYS_CTRL1_REG, 0x00);
  HAL_Delay(2);
}

uint16_t stmpe811_ReadID(uint16_t DeviceAddr)
{
  uint16_t id = 0;
  /* Odczyt ID (MSB i LSB) - POPRAWNA KOLEJNOŚĆ */
  id = (stmpe811_ReadReg(DeviceAddr, STMPE811_CHP_ID_MSB_REG) << 8);
  id |= stmpe811_ReadReg(DeviceAddr, STMPE811_CHP_ID_LSB_REG);
  return id;
}

void stmpe811_TS_Start(uint16_t DeviceAddr)
{
  uint8_t mode;

  /* 1. Włącz zegar ADC i TSC (SYS_CTRL2) */
  mode = stmpe811_ReadReg(DeviceAddr, STMPE811_SYS_CTRL2_REG);
  /* Zerujemy bity 0 (ADC_OFF) i 3 (TSC_OFF) */
  stmpe811_WriteReg(DeviceAddr, STMPE811_SYS_CTRL2_REG, mode & ~(0x01 | 0x08));

  /* 2. Konfiguracja ADC */
  /* ADC_CTRL1: Sample time = 80, 12-bit mod */
  stmpe811_WriteReg(DeviceAddr, STMPE811_ADC_CTRL1_REG, 0x49);
  HAL_Delay(2);
  /* ADC_CTRL2: ADC freq = 3.25 MHz */
  stmpe811_WriteReg(DeviceAddr, STMPE811_ADC_CTRL2_REG, 0x01);

  /* 3. Konfiguracja Panelu Dotykowego (TSC) */
  /* TSC_CFG: Ave=4, Touch Delay=500us, Settling=500us */
  stmpe811_WriteReg(DeviceAddr, STMPE811_TSC_CFG_REG, 0x9A);

  /* TSC_FRACT_XYZ: Format danych */
  stmpe811_WriteReg(DeviceAddr, STMPE811_TSC_FRACT_XYZ_REG, 0x01);

  /* TSC_I_DRIVE: Prąd 50mA */
  stmpe811_WriteReg(DeviceAddr, STMPE811_TSC_I_DRIVE_REG, 0x01);

  /* 4. Konfiguracja FIFO */
  stmpe811_WriteReg(DeviceAddr, STMPE811_FIFO_TH_REG, 0x01);
  stmpe811_WriteReg(DeviceAddr, STMPE811_FIFO_STA_REG, 0x01); /* Reset FIFO */
  stmpe811_WriteReg(DeviceAddr, STMPE811_FIFO_STA_REG, 0x00); /* Enable FIFO */

  /* 5. Włącz globalnie TS */
  /* TSC_CTRL: Enable TSC, Tryb XYZ */
  stmpe811_WriteReg(DeviceAddr, STMPE811_TSC_CTRL_REG, 0x01);

  /* Wyczyść przerwania */
  stmpe811_WriteReg(DeviceAddr, STMPE811_INT_STA_REG, 0xFF);
}

uint8_t stmpe811_TS_DetectTouch(uint16_t DeviceAddr)
{
  uint8_t state;
  uint8_t fifo_size;

  /* Metoda 1: Sprawdź status w rejestrze kontrolnym (bit 7) */
  state = (stmpe811_ReadReg(DeviceAddr, STMPE811_TSC_CTRL_REG) & 0x80);

  /* Metoda 2 (Pewniejsza): Sprawdź czy jest coś w FIFO */
  fifo_size = stmpe811_ReadReg(DeviceAddr, STMPE811_FIFO_SIZE_REG);

  /* Uznajemy dotyk, jeśli flaga jest ustawiona LUB są dane w FIFO */
  if(state > 0 || fifo_size > 0)
  {
      return 1;
  }
  return 0;
}

void stmpe811_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{
  uint8_t dataXYZ[4];

  /* WAŻNE: Czytamy 4 bajty w JEDNEJ transakcji (Burst Read).
     Czytanie ich osobno (po 1 bajcie) powoduje błędy pozycji!
     Startujemy od rejestru 0x4D (TSC_DATA_X) */
  I2Cx_ReadBuffer(DeviceAddr, STMPE811_TSC_DATA_X_REG, dataXYZ, 4);

  /* Obliczenia:
     Byte 0: X [11:4]
     Byte 1: X [3:0] | Y [11:8]
     Byte 2: Y [7:0]
  */
  uint32_t valX = (dataXYZ[0] << 4) | ((dataXYZ[1] & 0xF0) >> 4);
  uint32_t valY = ((dataXYZ[1] & 0x0F) << 8) | dataXYZ[2];

  /* Przypisanie do wskaźników */
  *X = (uint16_t)valX;
  *Y = (uint16_t)valY;

  /* Reset FIFO, aby przygotować układ na kolejny odczyt */
  stmpe811_WriteReg(DeviceAddr, STMPE811_FIFO_STA_REG, 0x01); /* Reset */
  stmpe811_WriteReg(DeviceAddr, STMPE811_FIFO_STA_REG, 0x00); /* Enable */
}

void stmpe811_TS_EnableIT(uint16_t DeviceAddr) {
  stmpe811_WriteReg(DeviceAddr, STMPE811_INT_EN_REG, 0x01);
  stmpe811_WriteReg(DeviceAddr, STMPE811_INT_CTRL_REG, 0x01);
}
void stmpe811_TS_DisableIT(uint16_t DeviceAddr) {
  stmpe811_WriteReg(DeviceAddr, STMPE811_INT_EN_REG, 0x00);
}
uint8_t stmpe811_TS_ITStatus(uint16_t DeviceAddr) {
  return (stmpe811_ReadReg(DeviceAddr, STMPE811_INT_STA_REG) & 0x01);
}
void stmpe811_TS_ClearIT(uint16_t DeviceAddr) {
  stmpe811_WriteReg(DeviceAddr, STMPE811_INT_STA_REG, 0xFF);
}

/* IO Functions Stubs (Uproszczone, by kompilator nie krzyczał) */
void stmpe811_IO_Start(uint16_t DeviceAddr, uint32_t IO_Pin) {}
uint8_t stmpe811_IO_Config(uint16_t DeviceAddr, uint32_t IO_Pin, IO_ModeTypedef IO_Mode) { return 0; }
void stmpe811_IO_WritePin(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t PinState) {}
uint32_t stmpe811_IO_ReadPin(uint16_t DeviceAddr, uint32_t IO_Pin) { return 0; }
void stmpe811_IO_EnableIT(uint16_t DeviceAddr) {}
void stmpe811_IO_DisableIT(uint16_t DeviceAddr) {}
uint32_t stmpe811_IO_ITStatus(uint16_t DeviceAddr, uint32_t IO_Pin) { return 0; }
void stmpe811_IO_ClearIT(uint16_t DeviceAddr, uint32_t IO_Pin) {}

/* STRUKTURY STEROWNIKA (Kluczowe dla kompatybilności) */
TS_DrvTypeDef stmpe811_ts_drv =
{
  stmpe811_Init,
  stmpe811_ReadID,
  stmpe811_Reset,
  stmpe811_TS_Start,
  stmpe811_TS_DetectTouch,
  stmpe811_TS_GetXY,
  stmpe811_TS_EnableIT,
  stmpe811_TS_ClearIT,
  stmpe811_TS_ITStatus,
  stmpe811_TS_DisableIT,
};

IO_DrvTypeDef stmpe811_io_drv =
{
  stmpe811_Init,
  stmpe811_ReadID,
  stmpe811_Reset,
  stmpe811_IO_Start,
  stmpe811_IO_Config,
  stmpe811_IO_WritePin,
  stmpe811_IO_ReadPin,
  stmpe811_IO_EnableIT,
  stmpe811_IO_DisableIT,
  stmpe811_IO_ITStatus,
  stmpe811_IO_ClearIT,
};

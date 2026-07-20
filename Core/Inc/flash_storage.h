#ifndef __FLASH_STORAGE_H
#define __FLASH_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

// STM32F429 Flash Sector 11 (Address Range: 0x080E0000 - 0x080FFFFF)
#define FLASH_HIGHSCORE_SECTOR      FLASH_SECTOR_11
#define FLASH_HIGHSCORE_ADDRESS     0x080E0000
#define FLASH_MAGIC_KEY             0x55AA1234

/**
 * @brief Reads the saved High Score percentage from STM32 internal Flash.
 * @return Saved High Score (0..100), or 0 if uninitialized.
 */
uint16_t Flash_ReadHighScore(void);

/**
 * @brief Erases Sector 11 and writes new High Score percentage.
 * @return 1 on success, 0 on error.
 */
uint8_t  Flash_SaveHighScore(uint16_t highScore);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_STORAGE_H */

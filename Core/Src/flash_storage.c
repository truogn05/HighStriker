#include "flash_storage.h"
#include "FreeRTOS.h"
#include "task.h"

uint16_t Flash_ReadHighScore(void)
{
    uint32_t magic = *(__IO uint32_t*)(FLASH_HIGHSCORE_ADDRESS);
    if (magic == FLASH_MAGIC_KEY) {
        uint32_t hs = *(__IO uint32_t*)(FLASH_HIGHSCORE_ADDRESS + 4);
        if (hs <= 9999) { // Support scores up to 9999%
            return (uint16_t)hs;
        }
    }
    return 0; // Default if uninitialized or invalid
}

uint8_t Flash_SaveHighScore(uint16_t highScore)
{
    // Avoid redundant Flash erases if value is already saved
    if (Flash_ReadHighScore() == highScore) {
        return 1;
    }

    uint8_t result = 0;

    taskENTER_CRITICAL();
    __disable_irq();

    HAL_FLASH_Unlock();

    // Clear pending FLASH flags before operation
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    FLASH_EraseInitTypeDef eraseConfig;
    uint32_t sectorError = 0;

    eraseConfig.TypeErase    = FLASH_TYPEERASE_SECTORS;
    eraseConfig.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    eraseConfig.Sector       = FLASH_HIGHSCORE_SECTOR;
    eraseConfig.NbSectors    = 1;

    if (HAL_FLASHEx_Erase(&eraseConfig, &sectorError) == HAL_OK) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_HIGHSCORE_ADDRESS, FLASH_MAGIC_KEY) == HAL_OK) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_HIGHSCORE_ADDRESS + 4, (uint32_t)highScore) == HAL_OK) {
                result = 1;
            }
        }
    }

    HAL_FLASH_Lock();

    __enable_irq();
    taskEXIT_CRITICAL();

    return result;
}

#include "flash_storage.h"

uint16_t Flash_ReadHighScore(void)
{
    uint32_t magic = *(__IO uint32_t*)(FLASH_HIGHSCORE_ADDRESS);
    if (magic == FLASH_MAGIC_KEY) {
        uint32_t hs = *(__IO uint32_t*)(FLASH_HIGHSCORE_ADDRESS + 4);
        if (hs <= 100) {
            return (uint16_t)hs;
        }
    }
    return 0; // Default if uninitialized or invalid
}

uint8_t Flash_SaveHighScore(uint16_t highScore)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef eraseConfig;
    uint32_t sectorError = 0;

    eraseConfig.TypeErase    = FLASH_TYPEERASE_SECTORS;
    eraseConfig.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    eraseConfig.Sector       = FLASH_HIGHSCORE_SECTOR;
    eraseConfig.NbSectors    = 1;

    if (HAL_FLASHEx_Erase(&eraseConfig, &sectorError) != HAL_OK) {
        HAL_FLASH_Lock();
        return 0;
    }

    // Write Magic Key and High Score Value
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_HIGHSCORE_ADDRESS, FLASH_MAGIC_KEY) == HAL_OK) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_HIGHSCORE_ADDRESS + 4, (uint32_t)highScore) == HAL_OK) {
            HAL_FLASH_Lock();
            return 1; // Success
        }
    }

    HAL_FLASH_Lock();
    return 0;
}

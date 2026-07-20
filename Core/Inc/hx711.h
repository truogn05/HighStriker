#ifndef __HX711_H
#define __HX711_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

typedef struct {
    GPIO_TypeDef* clk_port;
    uint16_t      clk_pin;
    GPIO_TypeDef* dout_port;
    uint16_t      dout_pin;
    uint8_t       gain; // 128 (Channel A gain 128), 64 (Channel A gain 64), 32 (Channel B gain 32)
} HX711_t;

/**
 * @brief Initialize ARM Cortex-M DWT Cycle Counter for microsecond delays.
 */
void HX711_InitDWT(void);

/**
 * @brief Initialize HX711 pin configuration and gain mode.
 */
void HX711_Init(HX711_t *hx, GPIO_TypeDef* clk_port, uint16_t clk_pin, GPIO_TypeDef* dout_port, uint16_t dout_pin, uint8_t gain);

/**
 * @brief Reads a 24-bit raw signed value from the HX711 module.
 * @param timeout_ms Timeout in milliseconds waiting for DOUT to go LOW.
 * @return Signed 32-bit raw value, or 0x80000000 on timeout error.
 */
int32_t HX711_ReadRaw(HX711_t *hx, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __HX711_H */

#include "hx711.h"

static void delay_us(uint32_t us)
{
    uint32_t startTicks = DWT->CYCCNT;
    uint32_t targetTicks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - startTicks) < targetTicks);
}

void HX711_InitDWT(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void HX711_Init(HX711_t *hx, GPIO_TypeDef* clk_port, uint16_t clk_pin, GPIO_TypeDef* dout_port, uint16_t dout_pin, uint8_t gain)
{
    hx->clk_port = clk_port;
    hx->clk_pin = clk_pin;
    hx->dout_port = dout_port;
    hx->dout_pin = dout_pin;
    hx->gain = gain;

    HAL_GPIO_WritePin(hx->clk_port, hx->clk_pin, GPIO_PIN_RESET);
}

int32_t HX711_ReadRaw(HX711_t *hx, uint32_t timeout_ms)
{
    uint32_t startTick = HAL_GetTick();
    
    // Wait until DOUT goes LOW
    while (HAL_GPIO_ReadPin(hx->dout_port, hx->dout_pin) == GPIO_PIN_SET) {
        if ((HAL_GetTick() - startTick) > timeout_ms) {
            return (int32_t)0x80000000; // Timeout error
        }
    }

    uint32_t raw = 0;
    uint8_t pulses = 25; // Default Channel A Gain 128
    if (hx->gain == 64) pulses = 27;
    else if (hx->gain == 32) pulses = 26;

    __disable_irq(); // Lock interrupts to prevent timing disruption during bit-banging

    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(hx->clk_port, hx->clk_pin, GPIO_PIN_SET);
        delay_us(1);
        raw = (raw << 1);
        if (HAL_GPIO_ReadPin(hx->dout_port, hx->dout_pin) == GPIO_PIN_SET) {
            raw++;
        }
        HAL_GPIO_WritePin(hx->clk_port, hx->clk_pin, GPIO_PIN_RESET);
        delay_us(1);
    }

    // Additional clock pulses according to specified gain
    for (int i = 0; i < (pulses - 24); i++) {
        HAL_GPIO_WritePin(hx->clk_port, hx->clk_pin, GPIO_PIN_SET);
        delay_us(1);
        HAL_GPIO_WritePin(hx->clk_port, hx->clk_pin, GPIO_PIN_RESET);
        delay_us(1);
    }

    __enable_irq();

    // Sign extend 24-bit value to 32-bit signed integer
    if (raw & 0x00800000) {
        raw |= 0xFF000000;
    }

    return (int32_t)raw;
}

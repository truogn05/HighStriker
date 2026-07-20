#include "load_cell.h"
#include <stdlib.h>

void LoadCell_Init(LoadCell_t *lc, GPIO_TypeDef* clk_port, uint16_t clk_pin, GPIO_TypeDef* dout_port, uint16_t dout_pin)
{
    HX711_InitDWT();

    // Ensure DOUT pin is configured as Input with pull-up
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dout_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(dout_port, &GPIO_InitStruct);

    // Ensure SCK pin is configured as Output
    GPIO_InitStruct.Pin = clk_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(clk_port, &GPIO_InitStruct);

    HX711_Init(&lc->hx, clk_port, clk_pin, dout_port, dout_pin, 128);
    lc->offset = 0;
    lc->scale = 1.0f;
    lc->is_measuring = 0;
    lc->start_time = 0;
    lc->current_peak = 0;
    lc->prev_force = 0;
    lc->last_hit_time = 0;
}

void LoadCell_Tare(LoadCell_t *lc, uint8_t sample_count)
{
    if (sample_count == 0) sample_count = 1;
    int64_t sum = 0;
    uint8_t valid_samples = 0;

    for (uint8_t i = 0; i < sample_count; i++) {
        int32_t raw = HX711_ReadRaw(&lc->hx, 100);
        // Ignore timeout or floating open-circuit readings
        if (raw != (int32_t)0x80000000 && raw < 0x007EFF00 && raw > -8388600) {
            sum += raw;
            valid_samples++;
        }
        HAL_Delay(10);
    }

    if (valid_samples > 0) {
        lc->offset = (int32_t)(sum / valid_samples);
    }
}

void LoadCell_SetScale(LoadCell_t *lc, float scale)
{
    if (scale != 0.0f) {
        lc->scale = scale;
    }
}

uint8_t LoadCell_Process(LoadCell_t *lc, int32_t *out_peak_force)
{
    int32_t raw = HX711_ReadRaw(&lc->hx, 50);
    
    // 1. Filter out timeout or open-circuit / VCC disconnected saturation codes (0x7FFFFF)
    if (raw == (int32_t)0x80000000 || raw >= 0x007EFF00 || raw <= -8388600) {
        lc->prev_force = 0;
        return 0; // Ignore invalid / disconnected sensor readings
    }

    // Convert raw reading using Tare offset & Scale (abs delta handles both A+/A- wiring directions)
    int32_t diff = raw - lc->offset;
    if (diff < 0) {
        diff = -diff;
    }
    int32_t force = (int32_t)(diff / lc->scale);

    uint32_t now = HAL_GetTick();

    if (!lc->is_measuring) {
        int32_t delta_force = force - lc->prev_force;

        // 2. IMPULSE DETECTION & LOCKOUT:
        // Only trigger if force rises rapidly (dF/dt >= IMPULSE_MIN_DELTA), exceeds minimum threshold,
        // and post-hit lockout cooldown period (1 sec) has passed.
        if (delta_force >= IMPULSE_MIN_DELTA && force >= FORCE_TRIGGER_THRESHOLD && (now - lc->last_hit_time) >= POST_HIT_LOCKOUT_MS) {
            lc->is_measuring = 1;
            lc->start_time = now;
            lc->current_peak = force;
        }
    } else {
        if (force > lc->current_peak) {
            lc->current_peak = force;
        }

        // 3. Short 200ms impact measurement window
        if ((now - lc->start_time) >= PEAK_TIMEOUT_MS) {
            lc->is_measuring = 0;
            lc->last_hit_time = now;

            if (out_peak_force) {
                *out_peak_force = lc->current_peak;
            }
            lc->prev_force = force;
            return 1; // Completed a valid fast strike measurement
        }
    }

    lc->prev_force = force;
    return 0;
}

uint16_t Force_To_Percent(int32_t peak_force)
{
    if (peak_force <= 0) return 0;

    // Calculate exact percentage
    uint32_t percent = ((uint32_t)peak_force * 100) / FORCE_MAX_THRESHOLD;
    return (uint16_t)percent;
}

uint8_t Percent_To_Level(uint16_t percent)
{
    if (percent == 0) return 0;
    uint8_t level = (uint8_t)((percent + 9) / 10);
    if (level > 10) level = 10;
    return level;
}

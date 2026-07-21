#ifndef __LOAD_CELL_H
#define __LOAD_CELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hx711.h"

// Thresholds for High Striker hit detection
#ifndef FORCE_TRIGGER_THRESHOLD
#define FORCE_TRIGGER_THRESHOLD   75000   // Minimum force units to trigger a hit (30 points)
#endif

#ifndef IMPULSE_MIN_DELTA
#define IMPULSE_MIN_DELTA         2500    // Fast force rise required (dF/dt) to reject static pressing
#endif

#ifndef FORCE_MAX_THRESHOLD
#define FORCE_MAX_THRESHOLD       250000  // Force units corresponding to 100% (Level 10)
#endif

#define PEAK_TIMEOUT_MS           200     // 200ms impact window to capture maximum strike peak
#define POST_HIT_LOCKOUT_MS       1000    // 1 second cooldown between consecutive strikes

typedef struct {
    HX711_t hx;
    int32_t offset;
    float   scale;
    
    // Peak detection state variables
    uint8_t  is_measuring;
    uint32_t start_time;
    int32_t  current_peak;
    int32_t  prev_force;
    uint32_t last_hit_time;
} LoadCell_t;

void LoadCell_Init(LoadCell_t *lc, GPIO_TypeDef* clk_port, uint16_t clk_pin, GPIO_TypeDef* dout_port, uint16_t dout_pin);
void LoadCell_Tare(LoadCell_t *lc, uint8_t sample_count);
void LoadCell_SetScale(LoadCell_t *lc, float scale);

uint8_t LoadCell_Process(LoadCell_t *lc, int32_t *out_peak_force);

uint16_t Force_To_Percent(int32_t peak_force);
uint8_t  Percent_To_Level(uint16_t percent);

#ifdef __cplusplus
}
#endif

#endif /* __LOAD_CELL_H */

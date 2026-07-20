#ifndef __SCORE_INTERFACE_H
#define __SCORE_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    uint8_t  level;             // 0..10 (11 levels representing game0.png to game10.png)
    uint16_t percent;           // 0..999 (%)
    uint8_t  isNewPeak;         // 1 = New strike peak captured
    uint16_t highScorePercent;  // High Score (%)
    uint8_t  isGameActive;      // 1 = Screen2 is active, 0 = Screen1 / Inactive
} ScoreDisplayData_t;

#if defined(SIMULATOR) || defined(WIN32) || defined(_WIN32)
// Stub implementations for TouchGFX Designer PC Simulator
static inline void Score_Init(uint16_t initialHighScore) {}
static inline void Score_GetDisplayData(ScoreDisplayData_t *outData) {
    if (outData) {
        outData->level = 0;
        outData->percent = 0;
        outData->isNewPeak = 0;
        outData->highScorePercent = 95; // Default highscore for simulator preview
        outData->isGameActive = 0;
    }
}
static inline void Score_SetNewHit(uint8_t level, uint16_t percent) {}
static inline void Score_SetGameActive(uint8_t active) {}
static inline uint8_t Score_IsGameActive(void) { return 1; }
static inline void Score_ResetHighScore(void) {}
static inline void Score_ClearNewPeakFlag(void) {}
static inline uint16_t Score_GetHighScore(void) { return 95; }
#else
// Prototypes for STM32 Target Hardware
void Score_Init(uint16_t initialHighScore);
void Score_GetDisplayData(ScoreDisplayData_t *outData);
void Score_SetNewHit(uint8_t level, uint16_t percent);
void Score_SetGameActive(uint8_t active);
uint8_t Score_IsGameActive(void);
void Score_ResetHighScore(void);
void Score_ClearNewPeakFlag(void);
uint16_t Score_GetHighScore(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SCORE_INTERFACE_H */

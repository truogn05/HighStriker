#ifndef __SCORE_INTERFACE_H
#define __SCORE_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Main interface data structure shared between Firmware sensor tasks and TouchGFX GUI.
 */
typedef struct {
    uint8_t  level;             // 0..10 (11 levels representing game0.png to game10.png)
    uint16_t percent;           // 0..999 (%)
    uint8_t  isNewPeak;         // 1 = New strike peak captured
    uint16_t highScorePercent;  // Saved High Score (%)
    uint8_t  isGameActive;      // 1 = Screen2 is active, 0 = Screen1 / Inactive
} ScoreDisplayData_t;

/**
 * @brief Initialize global score interface state.
 */
void Score_Init(uint16_t initialHighScore);

/**
 * @brief Thread-safe getter function called inside TouchGFX Model::tick().
 */
void Score_GetDisplayData(ScoreDisplayData_t *outData);

/**
 * @brief Thread-safe setter function called by Game Logic / Sensor task upon a new strike hit.
 */
void Score_SetNewHit(uint8_t level, uint16_t percent);

/**
 * @brief Sets active game screen flag (1 when Screen2 active, 0 otherwise).
 */
void Score_SetGameActive(uint8_t active);

/**
 * @brief Returns whether the game on Screen2 is currently active.
 */
uint8_t Score_IsGameActive(void);

/**
 * @brief Resets High Score in RAM and Flash to 0%.
 */
void Score_ResetHighScore(void);

/**
 * @brief Resets the isNewPeak flag once the GUI has initiated the hit animation.
 */
void Score_ClearNewPeakFlag(void);

/**
 * @brief Returns the current saved High Score percentage.
 */
uint16_t Score_GetHighScore(void);

#ifdef __cplusplus
}
#endif

#endif /* __SCORE_INTERFACE_H */

#include "score_interface.h"
#include "flash_storage.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile ScoreDisplayData_t g_scoreDisplay = {0, 0, 0, 0, 0};

void Score_Init(uint16_t initialHighScore)
{
    g_scoreDisplay.level = 0;
    g_scoreDisplay.percent = 0;
    g_scoreDisplay.isNewPeak = 0;
    g_scoreDisplay.highScorePercent = initialHighScore;
    g_scoreDisplay.isGameActive = 0;
}

void Score_GetDisplayData(ScoreDisplayData_t *outData)
{
    if (!outData) return;

    taskENTER_CRITICAL();
    outData->level = g_scoreDisplay.level;
    outData->percent = g_scoreDisplay.percent;
    outData->isNewPeak = g_scoreDisplay.isNewPeak;
    outData->highScorePercent = g_scoreDisplay.highScorePercent;
    outData->isGameActive = g_scoreDisplay.isGameActive;
    taskEXIT_CRITICAL();
}

void Score_SetNewHit(uint8_t level, uint16_t percent)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.level = level;
    g_scoreDisplay.percent = percent;
    g_scoreDisplay.isNewPeak = 1;

    if (percent > g_scoreDisplay.highScorePercent) {
        g_scoreDisplay.highScorePercent = percent;
    }
    taskEXIT_CRITICAL();
}

void Score_SetGameActive(uint8_t active)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.isGameActive = active;
    taskEXIT_CRITICAL();
}

uint8_t Score_IsGameActive(void)
{
    uint8_t act;
    taskENTER_CRITICAL();
    act = g_scoreDisplay.isGameActive;
    taskEXIT_CRITICAL();
    return act;
}

void Score_ResetHighScore(void)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.highScorePercent = 0;
    taskEXIT_CRITICAL();
    Flash_SaveHighScore(0);
}

void Score_ClearNewPeakFlag(void)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.isNewPeak = 0;
    taskEXIT_CRITICAL();
}

uint16_t Score_GetHighScore(void)
{
    uint16_t hs;
    taskENTER_CRITICAL();
    hs = g_scoreDisplay.highScorePercent;
    taskEXIT_CRITICAL();
    return hs;
}

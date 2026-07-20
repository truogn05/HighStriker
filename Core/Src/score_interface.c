#include "score_interface.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile ScoreDisplayData_t g_scoreDisplay = {0, 0, 0, 0};

void Score_Init(uint16_t initialHighScore)
{
    g_scoreDisplay.level = 0;
    g_scoreDisplay.percent = 0;
    g_scoreDisplay.isNewPeak = 0;
    g_scoreDisplay.highScorePercent = initialHighScore;
}

void Score_GetDisplayData(ScoreDisplayData_t *outData)
{
    if (!outData) return;

    taskENTER_CRITICAL();
    outData->level = g_scoreDisplay.level;
    outData->percent = g_scoreDisplay.percent;
    outData->isNewPeak = g_scoreDisplay.isNewPeak;
    outData->highScorePercent = g_scoreDisplay.highScorePercent;
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

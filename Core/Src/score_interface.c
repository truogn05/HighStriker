#include "score_interface.h"
#include "flash_storage.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile ScoreDisplayData_t g_scoreDisplay = {0, 0, 0, 0, 0, 0, 1};

void Score_Init(uint16_t initialHighScore)
{
    g_scoreDisplay.level = 0;
    g_scoreDisplay.percent = 0;
    g_scoreDisplay.isNewPeak = 0;
    g_scoreDisplay.highScorePercent = initialHighScore;
    g_scoreDisplay.isGameActive = 0;
    g_scoreDisplay.resetScoreRequested = 0;
    g_scoreDisplay.isReadyForHit = 1;
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
    outData->resetScoreRequested = g_scoreDisplay.resetScoreRequested;
    outData->isReadyForHit = g_scoreDisplay.isReadyForHit;
    taskEXIT_CRITICAL();
}

void Score_SetNewHit(uint8_t level, uint16_t percent)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.level = level;
    g_scoreDisplay.percent = percent;
    g_scoreDisplay.isNewPeak = 1;
    g_scoreDisplay.isReadyForHit = 0; // Lock out further hits until B1 pressed
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

void Score_UpdateHighScore(uint16_t newHighScore)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.highScorePercent = newHighScore;
    taskEXIT_CRITICAL();
}

void Score_RequestNewTurn(void)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.level = 0;
    g_scoreDisplay.percent = 0;
    g_scoreDisplay.resetScoreRequested = 1;
    g_scoreDisplay.isReadyForHit = 1; // Arm system for next hit
    taskEXIT_CRITICAL();
}

void Score_ClearResetScoreFlag(void)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.resetScoreRequested = 0;
    taskEXIT_CRITICAL();
}

uint8_t Score_IsReadyForHit(void)
{
    uint8_t ready;
    taskENTER_CRITICAL();
    ready = g_scoreDisplay.isReadyForHit;
    taskEXIT_CRITICAL();
    return ready;
}

void Score_SetReadyForHit(uint8_t ready)
{
    taskENTER_CRITICAL();
    g_scoreDisplay.isReadyForHit = ready;
    taskEXIT_CRITICAL();
}

#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
    : currentFrame(0), tickCounter(0)
{
    startFrames[0] = &start0;
    startFrames[1] = &start1;
    startFrames[2] = &start2;
    startFrames[3] = &start3;
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    // Disable loadcell force measurement on Screen1
    Score_SetGameActive(0);

    // Reset frame animation
    currentFrame = 0;
    tickCounter = 0;

    // Read high score when entering Screen1 and update BestScore text
    ScoreDisplayData_t data;
    Score_GetDisplayData(&data);

    BestScore.invalidate();
    touchgfx::Unicode::snprintf(BestScoreBuffer, BESTSCORE_SIZE, "%d", data.highScorePercent);
    BestScore.invalidate();

    // Show initial start frame
    showFrame(currentFrame);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::onScoreUpdated(const ScoreDisplayData_t&)
{

}

void Screen1View::handleTickEvent()
{
    Screen1ViewBase::handleTickEvent();

    tickCounter++;
    if (tickCounter >= TICKS_PER_FRAME)
    {
        tickCounter = 0;
        currentFrame = (currentFrame + 1) % NUM_START_FRAMES; // Cycle 0 -> 1 -> 2 -> 3 -> 0
        showFrame(currentFrame);
    }
}

void Screen1View::showFrame(uint8_t index)
{
    if (index >= NUM_START_FRAMES)
    {
        return;
    }

    for (uint8_t i = 0; i < NUM_START_FRAMES; i++)
    {
        bool isCurrent = (i == index);
        startFrames[i]->setVisible(isCurrent);
        startFrames[i]->invalidate();
    }
}

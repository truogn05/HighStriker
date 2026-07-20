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

    // Show initial start frame
    showFrame(currentFrame);

    // Read high score ONCE when entering Screen1 and format text ONCE
    ScoreDisplayData_t data;
    Score_GetDisplayData(&data);
    touchgfx::Unicode::snprintf(BestScoreBuffer, BESTSCORE_SIZE, "%d%%", data.highScorePercent);
    BestScore.invalidate();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::onScoreUpdated(const ScoreDisplayData_t& data)
{
    // Screen1 does not update score or process hits while active
}

void Screen1View::handleTickEvent()
{
    Screen1ViewBase::handleTickEvent();

    tickCounter++;
    if (tickCounter >= TICKS_PER_FRAME)
    {
        tickCounter = 0;
        currentFrame = (currentFrame + 1) % NUM_START_FRAMES;
        showFrame(currentFrame);
    }
}

void Screen1View::showFrame(uint8_t index)
{
    // Step 1: Set visibility for ALL frames first (no invalidate yet)
    for (uint8_t i = 0; i < NUM_START_FRAMES; i++)
    {
        bool isCurrent = (i == index);
        startFrames[i]->setVisible(isCurrent);
    }

    // Step 2: Invalidate ONCE on the newly visible frame only.
    // This prevents a two-pass repaint (hide old -> black flash -> draw new)
    // by letting TouchGFX redraw the region in a single render pass.
    startFrames[index]->invalidate();
}

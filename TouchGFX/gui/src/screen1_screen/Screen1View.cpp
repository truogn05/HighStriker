#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
    : currentFrame(0), tickCounter(0), lastHighScore(0xFFFF)
{
    startFrames[0] = &start0;
    startFrames[1] = &start1;
    startFrames[2] = &start2;
    startFrames[3] = &start3;
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    currentFrame = 0;
    tickCounter = 0;
    lastHighScore = 0xFFFF; // Force initial text update

    showFrame(currentFrame);

    ScoreDisplayData_t data;
    Score_GetDisplayData(&data);
    updateBestScoreText(data.highScorePercent);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::onScoreUpdated(const ScoreDisplayData_t& data)
{
    // Only update BestScore text if the value actually changes
    if (data.highScorePercent != lastHighScore)
    {
        updateBestScoreText(data.highScorePercent);
    }
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
    for (uint8_t i = 0; i < NUM_START_FRAMES; i++)
    {
        startFrames[i]->setVisible(i == index);
    }
    // Invalidate active container once to prevent Z-order redraw flickering
    invalidate();
}

void Screen1View::updateBestScoreText(uint16_t highScorePercent)
{
    lastHighScore = highScorePercent;
    BestScore.invalidate();
    touchgfx::Unicode::snprintf(BestScoreBuffer, BESTSCORE_SIZE, "%d%%", highScorePercent);
    BestScore.resizeToCurrentText();
    BestScore.invalidate();
}

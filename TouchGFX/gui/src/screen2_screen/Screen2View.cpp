#include <gui/screen2_screen/Screen2View.hpp>

Screen2View::Screen2View()
    : stepIndex(0), tickCounter(0), animationRunning(true)
{
    gameFrames[0]  = &game0;
    gameFrames[1]  = &game1;
    gameFrames[2]  = &game2;
    gameFrames[3]  = &game3;
    gameFrames[4]  = &game4;
    gameFrames[5]  = &game5;
    gameFrames[6]  = &game6;
    gameFrames[7]  = &game7;
    gameFrames[8]  = &game8;
    gameFrames[9]  = &game9;
    gameFrames[10] = &game10;
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    // Reset animation mỗi lần vào lại Screen2 (từ Screen1 sang)
    stepIndex = 0;
    tickCounter = 0;
    animationRunning = true;
    showFrame(frameForStep(stepIndex));
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::handleTickEvent()
{
    Screen2ViewBase::handleTickEvent();

    if (!animationRunning)
    {
        return;
    }

    tickCounter++;
    if (tickCounter >= TICKS_PER_FRAME)
    {
        tickCounter = 0;
        stepIndex++;

        if (stepIndex >= TOTAL_STEPS)
        {
            // Dừng lại ở frame cuối (game0) sau khi lặp đủ 2 lần
            stepIndex = TOTAL_STEPS - 1;
            animationRunning = false;
        }

        showFrame(frameForStep(stepIndex));
    }
}

uint8_t Screen2View::frameForStep(uint16_t step) const
{
    uint16_t s = step % STEPS_PER_LOOP;
    if (s <= (NUM_GAME_FRAMES - 1))
    {
        return static_cast<uint8_t>(s);          // đi lên 0->10
    }
    return static_cast<uint8_t>(STEPS_PER_LOOP - 1 - s); // đi xuống 10->0
}

void Screen2View::showFrame(uint8_t index)
{
    for (uint8_t i = 0; i < NUM_GAME_FRAMES; i++)
    {
        gameFrames[i]->setVisible(i == index);
        gameFrames[i]->invalidate();
    }
}

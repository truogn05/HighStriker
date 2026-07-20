#include <gui/screen2_screen/Screen2View.hpp>

Screen2View::Screen2View()
    : state(STATE_INTRO_ANIMATION), stepIndex(0), currentLevel(0), targetLevel(0), lastPercent(0), holdCounter(0), tickCounter(0)
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

    Score.setWildcard(scoreBuffer);

    // Tắt nhận tín hiệu đo lực trong khi Intro đang chạy
    Score_SetGameActive(0);

    state = STATE_INTRO_ANIMATION;
    stepIndex = 0;
    tickCounter = 0;
    currentLevel = 0;
    targetLevel = 0;
    lastPercent = 0;
    holdCounter = 0;

    showFrame(frameForStep(stepIndex));
    updateScoreText(0);

    ScoreDisplayData_t data;
    Score_GetDisplayData(&data);
    updateHighScoreText(data.highScorePercent);
}

void Screen2View::tearDownScreen()
{
    // Tắt nhận tín hiệu cảm biến khi rời khỏi Screen2
    Score_SetGameActive(0);
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::onScoreUpdated(const ScoreDisplayData_t& data)
{
    updateHighScoreText(data.highScorePercent);

    // CHỈ cho phép nhận lực đập từ phần cứng khi đã ở trạng thái STATE_IDLE (đã chạy xong Intro)
    if (data.isNewPeak && state == STATE_IDLE)
    {
        targetLevel = data.level;
        lastPercent = data.percent;
        currentLevel = 0;
        state = STATE_RISING;
        tickCounter = 0;
        showFrame(0);
        updateScoreText(0);
    }
}

void Screen2View::handleClickEvent(const touchgfx::ClickEvent& event)
{
    Screen2ViewBase::handleClickEvent(event);
}

void Screen2View::handleTickEvent()
{
    Screen2ViewBase::handleTickEvent();

    switch (state)
    {
    case STATE_INTRO_ANIMATION:
        tickCounter++;
        if (tickCounter >= TICKS_PER_INTRO_FRAME)
        {
            tickCounter = 0;
            stepIndex++;

            if (stepIndex >= TOTAL_STEPS)
            {
                // Dừng intro sau khi lặp đủ 2 lần 0->10->0 và chuyển sang IDLE
                stepIndex = TOTAL_STEPS - 1;
                state = STATE_IDLE;
                currentLevel = 0;
                showFrame(0);
                updateScoreText(0);

                // CHỈ kích hoạt nhận tín hiệu cảm biến KHI INTRO ĐÃ CHẠY XONG!
                Score_SetGameActive(1);
            }
            else
            {
                showFrame(frameForStep(stepIndex));
            }
        }
        break;

    case STATE_RISING:
        tickCounter++;
        if (tickCounter >= TICKS_PER_STEP)
        {
            tickCounter = 0;
            if (currentLevel < targetLevel)
            {
                currentLevel++;
                showFrame(currentLevel);
                // Cập nhật số % tăng dần mượt mà theo độ cao cột lực
                uint16_t displayPercent = (uint16_t)((uint32_t)lastPercent * currentLevel / targetLevel);
                updateScoreText(displayPercent);
            }
            else
            {
                state = STATE_HOLD;
                holdCounter = 0;
                updateScoreText(lastPercent);
            }
        }
        break;

    case STATE_HOLD:
        holdCounter++;
        if (holdCounter >= 15) // Giữ đỉnh ngắn 0.25s để hiển thị rồi lập tức hạ xuống
        {
            state = STATE_DECAY;
            tickCounter = 0;
        }
        break;

    case STATE_DECAY:
        tickCounter++;
        if (tickCounter >= TICKS_PER_STEP)
        {
            tickCounter = 0;
            if (currentLevel > 0)
            {
                currentLevel--;
                showFrame(currentLevel);
            }
            else
            {
                state = STATE_IDLE;
                updateScoreText(0);
            }
        }
        break;

    case STATE_IDLE:
    default:
        break;
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

void Screen2View::updateScoreText(uint16_t percent)
{
    Score.invalidate(); // Invalidate old area
    touchgfx::Unicode::snprintf(scoreBuffer, SCORE_BUFFER_SIZE, "%d%%", percent);
    Score.invalidate(); // Keep fixed width & alignment intact (do not call resizeToCurrentText)
}

void Screen2View::updateHighScoreText(uint16_t highScorePercent)
{
    HighScore.invalidate(); // Invalidate old area
    touchgfx::Unicode::snprintf(HighScoreBuffer, HIGHSCORE_SIZE, "%d%%", highScorePercent);
    HighScore.invalidate(); // Keep fixed width & alignment intact (do not call resizeToCurrentText)
}

#include <gui/screen2_screen/Screen2View.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>

Screen2View::Screen2View()
    : state(STATE_INTRO_ANIMATION),
      activeStatusText(STATUS_NONE),
      stepIndex(0),
      currentLevel(0),
      targetLevel(0),
      lastPercent(0),
      holdCounter(0),
      tickCounter(0),
      currentHighScore(0),
      bgTickCounter(0),
      bgFrameIndex(0),
      stateTimer(0),
      alphaVal(255),
      alphaDir(-4)
{
    emptyFrames[0] = &empty0;
    emptyFrames[1] = &empty1;
    emptyFrames[2] = &empty2;
    emptyFrames[3] = &empty3;

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

    // Dam bao tat ca bitmap cho 4 hinh nen duoc khoi tao dung
    empty0.setBitmap(touchgfx::Bitmap(BITMAP_EMPTY0_ID));
    empty1.setBitmap(touchgfx::Bitmap(BITMAP_EMPTY1_ID));
    empty2.setBitmap(touchgfx::Bitmap(BITMAP_EMPTY2_ID));
    empty3.setBitmap(touchgfx::Bitmap(BITMAP_EMPTY3_ID));

    bgTickCounter = 0;
    bgFrameIndex = 0;
    showEmptyFrame(bgFrameIndex);

    Score.setWildcard(scoreBuffer);

    // Tat nhan tin hieu do luc khi Intro dang chay
    Score_SetGameActive(0);

    state = STATE_INTRO_ANIMATION;
    stepIndex = 0;
    tickCounter = 0;
    currentLevel = 0;
    targetLevel = 0;
    lastPercent = 0;
    holdCounter = 0;
    stateTimer = 0;
    alphaVal = 255;
    alphaDir = -4;

    showGameFrame(frameForStep(stepIndex));
    updateScoreText(0);

    // Ban dau hide text2 va tat ca cac o text thong bao tinh bên phai
    text2.setVisible(false);
    text2.invalidate();
    hideAllStatusTexts();

    ScoreDisplayData_t data;
    Score_GetDisplayData(&data);
    currentHighScore = data.highScorePercent;
    updateHighScoreText(currentHighScore);
}

void Screen2View::tearDownScreen()
{
    Score_SetGameActive(0);
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::onScoreUpdated(const ScoreDisplayData_t& data)
{
    // Xu ly khi nut B1 đuoc bam
    if (data.resetScoreRequested)
    {
        Score_ClearResetScoreFlag();
        updateScoreText(0);

        // Chi xu ly bat dau luong xac nhan (OK -> Ready... -> Go!) khi dang o STATE_WAIT_CONFIRM hoac STATE_RESULT_DISPLAY
        if (state == STATE_WAIT_CONFIRM || state == STATE_RESULT_DISPLAY)
        {
            startConfirmationFlow();
        }
    }

    // Cap nhat diem ky luc khi co thay doi
    if (data.highScorePercent != currentHighScore)
    {
        currentHighScore = data.highScorePercent;
        updateHighScoreText(currentHighScore);
    }

    // Nhan tin hieu dap khi game o trang thai STATE_GO_IDLE
    if (data.isNewPeak && state == STATE_GO_IDLE)
    {
        targetLevel = data.level;
        lastPercent = data.percent;
        currentLevel = 0;
        state = STATE_RISING;
        tickCounter = 0;
        showGameFrame(0);
        updateScoreText(0);
        Score_ClearNewPeakFlag();
    }
}

void Screen2View::handleClickEvent(const touchgfx::ClickEvent& event)
{
    Screen2ViewBase::handleClickEvent(event);
}

void Screen2View::updateBackgroundAnimation()
{
    bgTickCounter++;
    if (bgTickCounter >= TICKS_PER_BG_FRAME) // 20 ticks = 3 fps
    {
        bgTickCounter = 0;
        bgFrameIndex = (bgFrameIndex + 1) % NUM_EMPTY_FRAMES;
        showEmptyFrame(bgFrameIndex);
    }
}

void Screen2View::updateAlphaPulse()
{
    int16_t nextAlpha = static_cast<int16_t>(alphaVal) + alphaDir;
    if (nextAlpha <= 127)
    {
        nextAlpha = 127;
        alphaDir = 4;
    }
    else if (nextAlpha >= 255)
    {
        nextAlpha = 255;
        alphaDir = -4;
    }
    alphaVal = static_cast<uint8_t>(nextAlpha);

    // Bat nhay Alpha cho o text2 ("Are U Ready") khi text2 hien thi
    if (text2.isVisible())
    {
        text2.setAlpha(alphaVal);
        text2.invalidate();
    }

    // Bat nhay Alpha cho o text thong bao danh hieu dang active o STATE_RESULT_DISPLAY
    if (state == STATE_RESULT_DISPLAY)
    {
        touchgfx::TextArea* target = nullptr;
        if (activeStatusText == STATUS_BAD) target = &txtBad;
        else if (activeStatusText == STATUS_GREAT) target = &txtGreat;
        else if (activeStatusText == STATUS_EXCELLENT) target = &txtExcellent;

        if (target && target->isVisible())
        {
            target->setAlpha(alphaVal);
            target->invalidate();
        }
    }
}

void Screen2View::hideAllStatusTexts()
{
    txtOK.setVisible(false);        txtOK.invalidate();
    txtReady.setVisible(false);     txtReady.invalidate();
    txtGo.setVisible(false);        txtGo.invalidate();
    txtBad.setVisible(false);       txtBad.invalidate();
    txtGreat.setVisible(false);     txtGreat.invalidate();
    txtExcellent.setVisible(false); txtExcellent.invalidate();
    activeStatusText = STATUS_NONE;
}

void Screen2View::showStatusText(StatusTextType type, bool pulseAlpha)
{
    hideAllStatusTexts();
    activeStatusText = type;

    touchgfx::TextArea* target = nullptr;
    switch (type)
    {
    case STATUS_OK:        target = &txtOK; break;
    case STATUS_READY:     target = &txtReady; break;
    case STATUS_GO:        target = &txtGo; break;
    case STATUS_BAD:       target = &txtBad; break;
    case STATUS_GREAT:     target = &txtGreat; break;
    case STATUS_EXCELLENT: target = &txtExcellent; break;
    default: break;
    }

    if (target)
    {
        target->setVisible(true);
        if (!pulseAlpha)
        {
            target->setAlpha(255);
        }
        target->invalidate();
    }
}

void Screen2View::startConfirmationFlow()
{
    state = STATE_CONFIRMED_OK;
    stateTimer = 0;
    Score_SetGameActive(0); // Tat phan cung trong luc hien thi OK -> Ready...

    // An o text2 ("Are U Ready") khi bat dau luong xac nhan
    setAreUReadyText(false, false);

    // Bam B1 se hien txtOK ("OK")
    showStatusText(STATUS_OK, false);
}

void Screen2View::handleTickEvent()
{
    Screen2ViewBase::handleTickEvent();

    // 1. Luon cap nhat hieu ung nen 3 FPS
    updateBackgroundAnimation();

    // 2. Luon cap nhat hieu ung nhay alpha muot
    updateAlphaPulse();

    // 3. Quan ly luong trang thai chinh cua Screen2
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
                // Intro chay xong -> chuyen sang STATE_WAIT_CONFIRM
                stepIndex = TOTAL_STEPS - 1;
                state = STATE_WAIT_CONFIRM;
                currentLevel = 0;
                showGameFrame(0);
                updateScoreText(0);

                // Hien o text2 ("Are U Ready") co dinh ben trai voi hieu ung nhay alpha
                setAreUReadyText(true, true);
                hideAllStatusTexts();

                Score_SetGameActive(0); // Van giu phan cung ngat tin hieu
            }
            else
            {
                showGameFrame(frameForStep(stepIndex));
            }
        }
        break;

    case STATE_WAIT_CONFIRM:
        // Cho nguoi choi bam B1 (duoc bat trong onScoreUpdated -> startConfirmationFlow)
        break;

    case STATE_CONFIRMED_OK:
        stateTimer++;
        if (stateTimer >= DELAY_500MS_TICKS) // 0.5s = 30 ticks
        {
            state = STATE_READY_COUNTDOWN;
            stateTimer = 0;

            // Sau 0.5s OK: Hien txtReady ("Ready...")
            showStatusText(STATUS_READY, false);
        }
        break;

    case STATE_READY_COUNTDOWN:
        stateTimer++;
        if (stateTimer >= DELAY_500MS_TICKS) // 0.5s = 30 ticks
        {
            state = STATE_GO_IDLE;
            stateTimer = 0;

            // Sau 0.5s Ready: Hien txtGo ("Go!")
            showStatusText(STATUS_GO, false);

            // KICH HOAT NHAN TIN HIEU DAP TU PHAN CUNG!
            Score_SetGameActive(1);
            Score_SetReadyForHit(1);
        }
        break;

    case STATE_GO_IDLE:
        // Dang o trang thai san sang cho luc dap tu loadcell
        break;

    case STATE_RISING:
        tickCounter++;
        if (tickCounter >= TICKS_PER_STEP)
        {
            tickCounter = 0;
            if (currentLevel < targetLevel)
            {
                currentLevel++;
                showGameFrame(currentLevel);
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
        if (holdCounter >= 15) // Giu dinh 0.25s
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
                showGameFrame(currentLevel);
            }
            else
            {
                // Thanh bar da ha ve 0 hoan tat!
                // Cap nhat HighScore neu co ky luc moi
                if (lastPercent > currentHighScore)
                {
                    currentHighScore = lastPercent;
                    updateHighScoreText(currentHighScore);
                    Score_UpdateHighScore(currentHighScore);
                }

                // Chuyen sang STATE_RESULT_DISPLAY de hien thi thong bao danh hieu
                state = STATE_RESULT_DISPLAY;
                stateTimer = 0;
                alphaVal = 255;
                alphaDir = -4;

                // An text2 ("Are U Ready?") ban dau
                setAreUReadyText(false, false);

                if (lastPercent < 40)
                {
                    showStatusText(STATUS_BAD, true);
                }
                else if (lastPercent < 80)
                {
                    showStatusText(STATUS_GREAT, true);
                }
                else
                {
                    showStatusText(STATUS_EXCELLENT, true);
                }
            }
        }
        break;

    case STATE_RESULT_DISPLAY:
        stateTimer++;
        if (stateTimer == AREUREADY_DELAY_3S_TICKS) // Sau 3.0s = 180 ticks
        {
            // Hien lai "Are U Ready?" tren text2 (ben trai) nhay alpha, chu danh hieu ben phai VAN GIU NGUYEN!
            setAreUReadyText(true, true);
        }
        break;

    default:
        break;
    }
}

uint8_t Screen2View::frameForStep(uint16_t step) const
{
    uint16_t s = step % STEPS_PER_LOOP;
    if (s <= (NUM_GAME_FRAMES - 1))
    {
        return static_cast<uint8_t>(s);
    }
    return static_cast<uint8_t>(STEPS_PER_LOOP - 1 - s);
}

void Screen2View::showEmptyFrame(uint8_t index)
{
    for (uint8_t i = 0; i < NUM_EMPTY_FRAMES; i++)
    {
        emptyFrames[i]->setVisible(i == index);
        emptyFrames[i]->invalidate();
    }
}

void Screen2View::showGameFrame(uint8_t index)
{
    for (uint8_t i = 0; i < NUM_GAME_FRAMES; i++)
    {
        gameFrames[i]->setVisible(i == index);
        gameFrames[i]->invalidate();
    }
}

void Screen2View::updateScoreText(uint16_t percent)
{
    Score.invalidate();
    touchgfx::Unicode::snprintf(scoreBuffer, SCORE_BUFFER_SIZE, "%d", percent);
    Score.invalidate();
}

void Screen2View::updateHighScoreText(uint16_t highScorePercent)
{
    HighScore.invalidate();
    touchgfx::Unicode::snprintf(HighScoreBuffer, HIGHSCORE_SIZE, "%d", highScorePercent);
    HighScore.invalidate();
}

void Screen2View::setAreUReadyText(bool visible, bool pulseAlpha)
{
    text2.setVisible(visible);
    if (visible)
    {
        if (!pulseAlpha)
        {
            text2.setAlpha(255);
        }
    }
    text2.invalidate();
}

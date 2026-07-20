#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include "score_interface.h"

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void handleClickEvent(const touchgfx::ClickEvent& event);
    void onScoreUpdated(const ScoreDisplayData_t& data);

protected:
    static const uint8_t  NUM_GAME_FRAMES = 11;       // game0..game10
    static const uint8_t  SCORE_BUFFER_SIZE = 10;
    static const uint8_t  TICKS_PER_STEP = 3;         // Tốc độ nảy dâng cột: ~20 fps
    static const uint8_t  TICKS_PER_INTRO_FRAME = 6;  // Tốc độ intro: ~10 fps
    static const uint8_t  NUM_REPEATS = 2;            // Intro lặp 2 lần
    static const uint16_t STEPS_PER_LOOP = 21;        // 1 vòng = 0->10->0 = 21 bước
    static const uint16_t TOTAL_STEPS = 42;           // 21 * 2 = 42 bước

    enum ViewState
    {
        STATE_INTRO_ANIMATION,
        STATE_IDLE,
        STATE_RISING,
        STATE_HOLD,
        STATE_DECAY
    };

    touchgfx::Image* gameFrames[NUM_GAME_FRAMES];
    touchgfx::Unicode::UnicodeChar scoreBuffer[SCORE_BUFFER_SIZE];

    ViewState state;
    uint16_t  stepIndex;
    uint8_t   currentLevel;
    uint8_t   targetLevel;
    uint16_t  lastPercent;
    uint16_t  holdCounter;
    uint8_t   tickCounter;

    void showFrame(uint8_t index);
    uint8_t frameForStep(uint16_t step) const;
    void updateScoreText(uint16_t percent);
    void updateHighScoreText(uint16_t highScorePercent);
};

#endif // SCREEN2VIEW_HPP

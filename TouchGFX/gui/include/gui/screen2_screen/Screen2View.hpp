#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>
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
    static const uint8_t  NUM_EMPTY_FRAMES = 4;        // empty0..empty3 (nền)
    static const uint8_t  TICKS_PER_BG_FRAME = 20;     // 3 fps (60 ticks / 3 = 20 ticks)

    static const uint8_t  NUM_GAME_FRAMES = 11;        // game0..game10 (thanh bar)
    static const uint8_t  SCORE_BUFFER_SIZE = 10;
    static const uint8_t  TICKS_PER_STEP = 3;          // Tốc độ nảy dâng cột: ~20 fps
    static const uint8_t  TICKS_PER_INTRO_FRAME = 6;   // Tốc độ intro: ~10 fps
    static const uint8_t  NUM_REPEATS = 2;             // Intro lặp 2 lần
    static const uint16_t STEPS_PER_LOOP = 21;         // 1 vòng = 0->10->0 = 21 bước
    static const uint16_t TOTAL_STEPS = 42;            // 21 * 2 = 42 bước
    static const uint16_t DELAY_500MS_TICKS = 30;      // 0.5s = 30 ticks ở 60Hz
    static const uint16_t RESULT_HOLD_TICKS = 150;     // 2.5s = 150 ticks
    static const uint16_t AREUREADY_DELAY_3S_TICKS = 180; // 3.0s = 180 ticks phu hien are u ready

    enum ViewState
    {
        STATE_INTRO_ANIMATION,
        STATE_WAIT_CONFIRM,      // Chờ bấm B1, ô text2 hiện "Are U Ready?" cyan nháy alpha
        STATE_CONFIRMED_OK,      // Bấm B1 xong, ô txtOK hiện "OK" green, giữ 0.5s
        STATE_READY_COUNTDOWN,   // Sau 0.5s OK, ẩn text2, ô txtReady hiện "Ready..." yellow, giữ 0.5s
        STATE_GO_IDLE,           // Sau 0.5s Ready, ô txtGo hiện "Go!" pink, bật nhận tín hiệu đập
        STATE_RISING,
        STATE_HOLD,
        STATE_DECAY,
        STATE_RESULT_DISPLAY     // Thanh bar hạ về 0, hiện txtBad/txtGreat/txtExcellent nháy alpha
    };

    enum StatusTextType
    {
        STATUS_NONE,
        STATUS_OK,
        STATUS_READY,
        STATUS_GO,
        STATUS_BAD,
        STATUS_GREAT,
        STATUS_EXCELLENT
    };

    touchgfx::Image* emptyFrames[NUM_EMPTY_FRAMES];
    touchgfx::Image* gameFrames[NUM_GAME_FRAMES];
    touchgfx::Unicode::UnicodeChar scoreBuffer[SCORE_BUFFER_SIZE];

    ViewState state;
    StatusTextType activeStatusText;
    uint16_t  stepIndex;
    uint8_t   currentLevel;
    uint8_t   targetLevel;
    uint16_t  lastPercent;
    uint16_t  holdCounter;
    uint8_t   tickCounter;
    uint16_t  currentHighScore;
    bool      isHighScorePulsing;

    // Hiệu ứng nền empty0-3 (3 FPS)
    uint8_t   bgTickCounter;
    uint8_t   bgFrameIndex;

    // Dem thoi gian
    uint16_t  stateTimer;

    // Alpha pulsing effect (255..127)
    uint8_t   alphaVal;
    int8_t    alphaDir;

    void updateBackgroundAnimation();
    void updateAlphaPulse();
    void showEmptyFrame(uint8_t index);
    void showGameFrame(uint8_t index);
    uint8_t frameForStep(uint16_t step) const;
    void updateScoreText(uint16_t percent);
    void updateHighScoreText(uint16_t highScorePercent);
    void setAreUReadyText(bool visible, bool pulseAlpha);
    void hideAllStatusTexts();
    void showStatusText(StatusTextType type, bool pulseAlpha);
    void startConfirmationFlow();
};

#endif // SCREEN2VIEW_HPP

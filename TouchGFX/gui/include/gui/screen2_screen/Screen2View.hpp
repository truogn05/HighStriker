#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

protected:
    static const uint8_t  NUM_GAME_FRAMES = 11;  // game0..game10
    static const uint8_t  TICKS_PER_FRAME = 6;   // 60Hz tick / 10fps
    static const uint8_t  NUM_REPEATS     = 2;   // lặp 2 lần
    // 1 vòng = 0->10->0 = 21 bước (0..10 rồi 9..0)
    static const uint16_t STEPS_PER_LOOP  = 2 * (NUM_GAME_FRAMES - 1) + 1; // 21
    static const uint16_t TOTAL_STEPS     = STEPS_PER_LOOP * NUM_REPEATS; // 42

    touchgfx::Image* gameFrames[NUM_GAME_FRAMES];
    uint16_t stepIndex;
    uint8_t  tickCounter;
    bool     animationRunning;

    void showFrame(uint8_t index);
    uint8_t frameForStep(uint16_t step) const;
};

#endif // SCREEN2VIEW_HPP

#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "score_interface.h"

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    void onScoreUpdated(const ScoreDisplayData_t& data);

protected:
    static const uint8_t NUM_START_FRAMES = 4;   // start0..start3
    static const uint8_t TICKS_PER_FRAME  = 20;  // 60Hz tick / 3fps

    touchgfx::Image* startFrames[NUM_START_FRAMES];
    uint8_t  currentFrame;
    uint8_t  tickCounter;
    uint16_t lastHighScore;

    void showFrame(uint8_t index);
    void updateBestScoreText(uint16_t highScorePercent);
};

#endif // SCREEN1VIEW_HPP

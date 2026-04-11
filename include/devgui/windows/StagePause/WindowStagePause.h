#pragma once

#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateCtrl.h"

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameStagePause = "Stage Pauser";

class WindowStagePause : public WindowBase {
private:
    bool mIsStagePaused = false;
    bool mIsPauseAudio = false;
    bool mIsAdvanceFrame = false;
    al::NerveKeeper mPreviousNerveKeeper = {nullptr, nullptr, 0};
    al::NerveStateCtrl::State* mPreviousState = nullptr;
    bool mIsPausing = false;
    bool mIsUseBuffer = false;
    bool mCamDuringPause = false;
    void runFrame();

public:
    WindowStagePause(DevGuiManager* parent, const char* winName, bool isActiveByDefault);
    bool tryUpdateWinDisplay() override;
    void updateWin() override;
    bool tryAdvanceFrame();
    bool tryPauseScene();
    bool tryUnpauseScene();
    void tryTogglePause();

    bool getStagePaused() { return mIsStagePaused; }

    bool isPausing() { return mIsPausing; };

    bool isCamDuringPause() { return mCamDuringPause; };
};

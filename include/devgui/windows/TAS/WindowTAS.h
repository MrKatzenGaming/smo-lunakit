#pragma once

#include "devgui/categories/CategoryBase.h"
#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameTas = "TAS";

class WindowTAS : public WindowBase {
public:
    WindowTAS(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    //    void updateWin() override;
    bool tryUpdateWinDisplay() override;

private:
    bool isStartTAS = false;
    bool isStartRecord = false;
    bool isStartReplay = false;
};

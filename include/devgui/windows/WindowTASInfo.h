#pragma once

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameTasInfo = "TAS Info";

class WindowTASInfo : public WindowBase {
public:
    WindowTASInfo(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;
};

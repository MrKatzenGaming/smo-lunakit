#pragma once

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameDupHotkeys = "Dup Hotkeys";

class WindowDupHotkeys : public WindowBase {
public:
    WindowDupHotkeys(DevGuiManager* parent, const char* winName, bool isActiveByDefault);
};

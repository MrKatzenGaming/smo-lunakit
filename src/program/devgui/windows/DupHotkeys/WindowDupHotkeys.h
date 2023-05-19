#pragma once

#include "program/devgui/windows/WindowBase.h"

__attribute__((used)) static const char* dupHotkeysWindowName = "Dup Hotkeys";

class WindowDupHotkeys : public WindowBase {
public:
    WindowDupHotkeys(DevGuiManager* parent, const char* winName, bool isActiveByDefault);
};
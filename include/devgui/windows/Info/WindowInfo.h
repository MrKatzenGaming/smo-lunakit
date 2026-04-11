
/*
    - LunaKit Info Viewer Window -

    This window displays general information about various parts of the game
    Uses categories

    https://github.com/Amethyst-szs/smo-lunakit/wiki#info-viewer
*/

#pragma once

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameInfo = "Info Viewer";

class WindowInfo : public WindowBase {
public:
    WindowInfo(DevGuiManager* parent, const char* winName, bool isActiveByDefault);
};

#pragma once

#include "devgui/windows/Hitboxes/CollisionDrawer.h"
#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameHitboxes = "Hitboxes";

class WindowHitboxes : public WindowBase {
public:
    WindowHitboxes(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;

private:
    bool mEnabled = false;
};

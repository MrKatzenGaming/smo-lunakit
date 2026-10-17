#pragma once

#include "devgui/DevGuiManager.h"
#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameHitboxes = "Hitboxes";

class WindowHitboxes : public WindowBase {
public:
    WindowHitboxes(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    bool tryUpdateWinDisplay() override;

    static bool shouldDrawCollision() {
        auto window = SEAD_INSTANCE(DevGuiManager)->getWindow<WindowHitboxes>(windowNameHitboxes);
        return window->mEnabled && window->mDrawCollision;
    }

private:
    bool mEnabled = false;

    bool mDrawCollision = true;
};

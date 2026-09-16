#pragma once

#include "devgui/windows/WindowBase.h"

#include "math/seadVector.h"
#include "math/seadVectorFwd.h"

__attribute__((used)) static const char* windowNameFreecam = "Freecam";

class WindowFreecam : public WindowBase {
public:
    WindowFreecam(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;

    static void installHooks();
    
    bool mEnabled = false;
    f32 mMoveSpeed = 10.0f;
    f32 mRotateSpeed = 1.0f;

    sead::Vector3f mOrigPos;
    sead::Vector3f mOrigAt;
    sead::Vector3f mOrigUp;

    sead::Vector3f mFreePos = {0.0f, 0.0f, 0.0f};
    sead::Vector3f mFreeAt = {0.0f, 0.0f, 1.0f};
    sead::Vector3f mFreeUp = {0.0f, 1.0f, 0.0f};
};


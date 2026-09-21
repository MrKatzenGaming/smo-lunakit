#pragma once

#include "sead/math/seadVector.h"

#include "al/Library/Camera/CameraPoseUpdater.h"
#include "al/Library/Projection/Projection.h"

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameFreecam = "Freecam";
__attribute__((used)) static const char* modeNames[] = {"First Person", "Orbit Player"};

class WindowFreecam : public WindowBase {
public:
    enum struct Mode : u8 {
        FirstPerson,
        OrbitPlayer,
    };

    WindowFreecam(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;

    static void installHooks();

    bool mEnabled = false;
    Mode mMode = Mode::FirstPerson;

    f32 mMoveSpeed = 50.0f;
    f32 mRotateSpeed = 2.0f;
    f32 mZoomSpeed = 20.0f;

    sead::Vector3f mPosition = sead::Vector3f::zero;
    sead::Vector2f mRotation = sead::Vector2f::zero;
    f32 mZoom = 1000.0f;

    f32 mNearClip = 25.0f;
    f32 mFarClip = 500'000.0f;
    f32 mFovyDegree = 40.0f;

    sead::Vector3f mOrigPos;
    sead::Vector3f mOrigAt;
    sead::Vector3f mOrigUp;
    al::Projection mOrigProj;
};

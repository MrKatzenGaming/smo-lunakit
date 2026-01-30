#pragma once

#include "custom/al/graphics/GBufferArray.h"

#include "devgui/windows/WindowBase.h"
#include "imgui.h"

__attribute__((used)) static const char* gbufferWindowName = "Frame Buffers";

struct GBufferSet {
    al::GBufferArray* mBuffer = nullptr;
    bool mEnable = false;
    int mShowType = 0;
    int mChannel = 0;
};

class WindowGBuffer : public WindowBase {
public:
    WindowGBuffer(DevGuiManager* parent, const char* winName, bool isActiveByDefault);
    bool tryUpdateWinDisplay() override;

    ImGuiComboFlags mComboFlags;
};

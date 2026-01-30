#pragma once

#include "sead/controller/seadControllerMgr.h"
#include "sead/math/seadVectorFwd.h"

#include "custom/al/Pad/NpadController.h"

#include "devgui/windows/WindowBase.h"
#include "imgui.h"

__attribute__((used)) static const char* inputWindowName = "Input Display";

class WindowInput : public WindowBase {
public:
    WindowInput(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;

    void drawInputDisplay();
    void drawInputDisplayP2();

    bool isEnabled() { return mIsEnabled; }

    void setEnabled(bool enabled) { mIsEnabled = enabled; }

    bool is2P() { return mIs2P; }

    void set2P(bool is2P) { mIs2P = is2P; }

    ImVec2 getPos() { return mPos; }

    void setPos(const ImVec2& pos) { mPos = pos; }

    ImVec2 getPos2() { return mPos2; }

    void setPos2(const ImVec2& pos) { mPos2 = pos; }

private:
    constexpr static ImVec4 sInputDisplayColors[]{
        {0, 0, 0, 0},          // None
        {255, 255, 255, 255},  // White
        {128, 128, 128, 255},  // Gray
        {0, 0, 0, 255},        // Black
        {235, 64, 52, 255},    // Red
        {52, 58, 235, 255},    // Blue
        {235, 210, 52, 255},   // Yellow
        {58, 235, 52, 255},    // Green
        {235, 152, 52, 255},   // Orange
        {113, 52, 235, 255},   // Purple
        {225, 52, 235, 255},   // Pink
        {52, 235, 235, 255}    // Light Blue
    };
    enum class InputDisplayColor : u8 { None, White, Gray, Black, Red, Blue, Yellow, Green, Orange, Purple, Pink, LightBlue };

    static ImU32 makeColor(const ImVec4& color) { return IM_COL32(u8(color.x), u8(color.y), u8(color.z), u8(color.w)); }

    static ImVec4 getInputDisplayColor(InputDisplayColor color) { return sInputDisplayColors[int(color)]; }

    void drawButton(const ImVec2& pos, bool callback, float radius = 8, ImU32 color = makeColor(getInputDisplayColor(InputDisplayColor::White)),
                    ImU32 pressedColor = makeColor(getInputDisplayColor(InputDisplayColor::Pink)));
    void drawButtonRect(const ImVec2& pos, bool callback, ImU32 color = makeColor(getInputDisplayColor(InputDisplayColor::White)),
                        ImU32 pressedColor = makeColor(getInputDisplayColor(InputDisplayColor::Pink)));

    ImVec2 mPos = ImVec2(1000, 500);
    ImVec2 mPos2 = ImVec2(1000, 800);
    bool mIsEnabled = false;
    bool mIs2P = false;

    sead::ControllerMgr* controllerMgr;
    al::NpadController* controller;
    al::NpadController* controller2;
    sead::BitFlag<u32> padHold;
    sead::BitFlag<u32> padHold2;
    sead::Vector2f mLStick;
    sead::Vector2f mRStick;
    sead::Vector2f mLStick2;
    sead::Vector2f mRStick2;
};

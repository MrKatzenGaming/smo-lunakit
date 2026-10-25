#include "devgui/windows/WindowInput.h"

#include "sead/controller/seadControllerMgr.h"
#include "sead/prim/seadBitFlag.h"

#include "Library/Controller/NpadController.h"
#include "al/Library/Controller/InputFunction.h"

#include "devgui/DevGuiManager.h"
#include "devgui/savedata/DevGuiSaveData.h"
#include "devgui/windows/WindowBase.h"
#include "devgui/windows/WindowStagePause.h"
#include "imgui.h"
#include "smo-tas/STAS.h"

WindowInput::WindowInput(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {}

using InputCallback = bool;

void WindowInput::drawButton(const ImVec2& pos, InputCallback callback, float radius, ImU32 color, ImU32 pressedColor) {
    const ImU32 col = callback ? pressedColor : color;
    ImGui::GetForegroundDrawList()->AddCircleFilled(pos, radius, col, 16);
}

void WindowInput::drawButtonRect(const ImVec2& pos, InputCallback callback, ImU32 color, ImU32 pressedColor) {
    const ImU32 col = callback ? pressedColor : color;
    const ImVec2 min(pos.x - 13, pos.y - 6);
    const ImVec2 max(pos.x + 13, pos.y + 6);
    ImGui::GetForegroundDrawList()->AddRectFilled(min, max, col, 10.0f);
}

void WindowInput::drawDisplay(ImVec2 posIn, sead::BitFlag<u32> padHold, sead::Vector2f lStick, sead::Vector2f rStick, const char* player,
                              const char* taser) {
    ImVec2 pos = posIn;
    pos.x -= 200;
    pos.y -= 100;

    ImVec4 color = getInputDisplayColor(InputDisplayColor::Black);
    color.w = 128;
    ImGui::GetForegroundDrawList()->AddRectFilled({pos.x - 50, pos.y - (mIs2P ? 150 : 120)}, {pos.x + 200, pos.y + 90}, makeColor(color), 20.0f);

    float fontSize = 30.f;
    ImVec2 textSizePlayer = ImGui::GetFont()->CalcTextSizeA(fontSize, 500, 500, player);
    if (mIs2P) {
        ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontSize, {pos.x + 80 - (textSizePlayer[0] / 2), pos.y - 120 - textSizePlayer[1]},
                                                makeColor(getInputDisplayColor(InputDisplayColor::White)), player);
    }
    if (STAS* tas = STAS::instance(); tas && tas->hasScript() && tas->isRunning()) {
        ImVec2 textSizeTASer = ImGui::GetFont()->CalcTextSizeA(fontSize, 500, 500, taser);
        ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontSize, {pos.x + 80 - (textSizeTASer[0] / 2), pos.y - 120},
                                                makeColor(getInputDisplayColor(InputDisplayColor::White)), taser);
    }

    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 leftPos = {pos.x + lStick.x * 30, pos.y - lStick.y * 30};
    drawButton(leftPos, padHold.isOnBit(al::NpadController::cPadIdx_1), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y += 30;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_Up));
    pos.y += 30;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_Down));
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_Left));
    pos.x += 30;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_Right));

    pos.x += 60;
    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 rightPos = {pos.x + rStick.x * 30, pos.y - rStick.y * 30};
    drawButton(rightPos, padHold.isOnBit(al::NpadController::cPadIdx_2), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y -= 60;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_X));
    pos.y += 30;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_B));
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_Y));
    pos.x += 30;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_A));

    pos.y -= 10;
    pos.x -= 75;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_Plus), 5);
    pos.x -= 40;
    drawButton(pos, padHold.isOnBit(al::NpadController::cPadIdx_Minus), 5);

    pos = posIn;
    pos.x -= 200;
    pos.y -= 160;
    drawButtonRect(pos, padHold.isOnBit(al::NpadController::cPadIdx_L));
    pos.y -= 16;
    drawButtonRect(pos, padHold.isOnBit(al::NpadController::cPadIdx_ZL));
    pos.x += 155;
    drawButtonRect(pos, padHold.isOnBit(al::NpadController::cPadIdx_ZR));
    pos.y += 16;
    drawButtonRect(pos, padHold.isOnBit(al::NpadController::cPadIdx_R));
}

void WindowInput::drawInputDisplay() {
    if (!mIsEnabled)
        return;

    drawDisplay(mPos, padHold, mLStick, mRStick, "Mario", STAS::instance()->getCurMarTASer());
}

void WindowInput::drawInputDisplayP2() {
    if (!mIsEnabled || !mIs2P)
        return;

    drawDisplay(mPos2, padHold2, mLStick2, mRStick2, "Cappy", STAS::instance()->getCurCapTASer());
}

void WindowInput::updateWin() {
    WindowBase::updateWin();
    if (!mIsEnabled)
        return;
    controllerMgr = sead::ControllerMgr::instance();
    WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
    // STAS* tas = STAS::instance();

    controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(0));
    if (mIs2P)
        controller2 = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(1));

    padHold = controller->mPadHold;
    mLStick = controller->mLeftStick;
    mRStick = controller->mRightStick;
    if (mIs2P) {
        padHold2 = controller2->mPadHold;
        mLStick2 = controller2->mLeftStick;
        mRStick2 = controller2->mRightStick;
    }
}

bool WindowInput::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;
    DevGuiSaveData* saveData = DevGuiManager::instance()->getSaveData();

    if (ImGui::Checkbox("Enable", &mIsEnabled))
        saveData->queueSaveWrite();
    ImGui::PushItemWidth(100);
    ImGui::PushID("1Paaaaaaaaaa");
    if (ImGui::DragFloat(" ", &mPos.x, 1, 200, 1600, "%.1f"))
        saveData->queueSaveWrite();
    ImGui::PopID();
    ImGui::PushID("1P2aaaaaaaaaa");

    ImGui::SameLine();
    if (ImGui::DragFloat("Position", &mPos.y, 1, 200, 910, "%.1f"))
        saveData->queueSaveWrite();
    ImGui::PopID();
    ImGui::PopItemWidth();

    if (ImGui::Checkbox("Enable P2", &mIs2P))
        saveData->queueSaveWrite();
    if (mIs2P) {
        ImGui::PushItemWidth(100);
        ImGui::PushID("2Paaaaaaaaaa");
        if (ImGui::DragFloat(" ", &mPos2.x, 1, 200, 1600, "%.1f"))
            saveData->queueSaveWrite();
        ImGui::PopID();
        ImGui::PushID("2P2aaaaaaaaaa");
        ImGui::SameLine();
        if (ImGui::DragFloat("Position P2", &mPos2.y, 1, 200, 910, "%.1f"))
            saveData->queueSaveWrite();
        ImGui::PopID();
        ImGui::PopItemWidth();
    }

    return true;
}

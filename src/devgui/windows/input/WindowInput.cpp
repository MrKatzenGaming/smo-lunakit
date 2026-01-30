#include "devgui/windows/input/WindowInput.h"

#include "sead/controller/seadControllerMgr.h"
#include "sead/prim/seadBitFlag.h"

#include "Library/Controller/InputFunction.h"

#include "devgui/DevGuiManager.h"
#include "devgui/savedata/DevGuiSaveData.h"
#include "devgui/windows/StagePause/WindowStagePause.h"
#include "devgui/windows/WindowBase.h"
#include "imgui.h"
#include "smo-tas/TAS.h"

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

void WindowInput::drawInputDisplay() {
    if (!mIsEnabled)
        return;

    ImVec2 pos = mPos;
    pos.x -= 200;
    pos.y -= 100;

    if (true) {
        ImVec4 color = getInputDisplayColor(InputDisplayColor::Black);
        color.w = 128;
        ImGui::GetForegroundDrawList()->AddRectFilled({pos.x - 50, pos.y - 100}, {pos.x + 200, pos.y + 100}, makeColor(color), 20.0f);
    }

    if (mIs2P)
        ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 30.f, {pos.x + 20, pos.y - 100},
                                                makeColor(getInputDisplayColor(InputDisplayColor::White)), "Player 1");

    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 leftPos = {pos.x + mLStick.x * 30, pos.y - mLStick.y * 30};  // stickl
    drawButton(leftPos, padHold.isOnBit(controller->cPadIdx_1), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y += 30;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_Up));  // up
    pos.y += 30;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_Down));  // down
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_Left));  // left
    pos.x += 30;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_Right));  // right

    pos.x += 60;
    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 rightPos = {pos.x + mRStick.x * 30, pos.y - mRStick.y * 30};  // stickr
    drawButton(rightPos, padHold.isOnBit(controller->cPadIdx_2), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y -= 60;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_X));  // x
    pos.y += 30;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_B));  // b
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_Y));  // y
    pos.x += 30;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_A));  // a

    pos.y -= 10;
    pos.x -= 75;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_Plus), 5);  // plus
    pos.x -= 40;
    drawButton(pos, padHold.isOnBit(controller->cPadIdx_Minus), 5);  // minus

    pos = mPos;
    pos.x -= 200;
    pos.y -= 160;
    drawButtonRect(pos, padHold.isOnBit(controller->cPadIdx_L));  // l
    pos.y -= 16;
    drawButtonRect(pos, padHold.isOnBit(controller->cPadIdx_ZL));  // zl
    pos.x += 155;
    drawButtonRect(pos, padHold.isOnBit(controller->cPadIdx_ZR));  // zr
    pos.y += 16;
    drawButtonRect(pos, padHold.isOnBit(controller->cPadIdx_R));  // r
}

void WindowInput::drawInputDisplayP2() {
    if (!mIsEnabled || !mIs2P)
        return;

    ImVec2 pos = mPos2;
    pos.x -= 200;
    pos.y -= 100;

    if (true) {
        ImVec4 color = getInputDisplayColor(InputDisplayColor::Black);
        color.w = 128;
        ImGui::GetForegroundDrawList()->AddRectFilled({pos.x - 50, pos.y - 100}, {pos.x + 200, pos.y + 100}, makeColor(color), 20.0f);
    }
    ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 30.f, {pos.x + 20, pos.y - 100},
                                            makeColor(getInputDisplayColor(InputDisplayColor::White)), "Player 2");

    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 leftPos = {pos.x + mLStick2.x * 30, pos.y - mLStick2.y * 30};  // stickl
    drawButton(leftPos, padHold2.isOnBit(controller->cPadIdx_1), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y += 30;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_Up));  // up
    pos.y += 30;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_Down));  // down
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_Left));  // left
    pos.x += 30;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_Right));  // right

    pos.x += 60;
    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 rightPos = {pos.x + mRStick2.x * 30, pos.y - mRStick2.y * 30};  // stickr
    drawButton(rightPos, padHold2.isOnBit(controller->cPadIdx_2), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y -= 60;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_X));  // x
    pos.y += 30;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_B));  // b
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_Y));  // y
    pos.x += 30;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_A));  // a

    pos.y -= 10;
    pos.x -= 75;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_Plus), 5);  // plus
    pos.x -= 40;
    drawButton(pos, padHold2.isOnBit(controller2->cPadIdx_Minus), 5);  // minus

    pos = mPos2;
    pos.x -= 200;
    pos.y -= 160;
    drawButtonRect(pos, padHold2.isOnBit(controller2->cPadIdx_L));  // l
    pos.y -= 16;
    drawButtonRect(pos, padHold2.isOnBit(controller2->cPadIdx_ZL));  // zl
    pos.x += 155;
    drawButtonRect(pos, padHold2.isOnBit(controller2->cPadIdx_ZR));  // zr
    pos.y += 16;
    drawButtonRect(pos, padHold2.isOnBit(controller2->cPadIdx_R));  // r
}

void WindowInput::updateWin() {
    if (!mIsEnabled)
        return;
    controllerMgr = sead::ControllerMgr::instance();
    WindowStagePause* win = (WindowStagePause*)DevGuiManager::instance()->getWindow("Stage Pauser");
    TAS* tas = TAS::instance();

    controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(0));
    if (mIs2P)
        controller2 = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(1));

    if (controller && tas) {
        padHold = win->getStagePaused() ? (tas->isRunning() && !tas->getScript()->mFrames[tas->getFrameIndex()].mSecondPlayer ?
                                               (sead::BitFlag<u32>)tas->getScript()->mFrames[tas->getFrameIndex()].mButtons :
                                               padHold) :
                                          controller->mPadHold;
        mLStick = win->getStagePaused() ? (tas->isRunning() && !tas->getScript()->mFrames[tas->getFrameIndex()].mSecondPlayer ?
                                               tas->getScript()->mFrames[tas->getFrameIndex()].mLeftStick :
                                               mLStick) :
                                          controller->mLeftStick;
        mRStick = win->getStagePaused() ? (tas->isRunning() && !tas->getScript()->mFrames[tas->getFrameIndex()].mSecondPlayer ?
                                               tas->getScript()->mFrames[tas->getFrameIndex()].mRightStick :
                                               mRStick) :
                                          controller->mRightStick;
    }
    if (controller2 && tas) {
        padHold2 = win->getStagePaused() ? (tas->isRunning() && tas->getScript()->mFrames[tas->getFrameIndex()].mSecondPlayer ?
                                                (sead::BitFlag<u32>)tas->getScript()->mFrames[tas->getFrameIndex()].mButtons :
                                                padHold2) :
                                           controller2->mPadHold;
        mLStick2 = win->getStagePaused() ? (tas->isRunning() && tas->getScript()->mFrames[tas->getFrameIndex()].mSecondPlayer ?
                                                tas->getScript()->mFrames[tas->getFrameIndex()].mLeftStick :
                                                mLStick2) :
                                           controller2->mLeftStick;
        mRStick2 = win->getStagePaused() ? (tas->isRunning() && tas->getScript()->mFrames[tas->getFrameIndex()].mSecondPlayer ?
                                                tas->getScript()->mFrames[tas->getFrameIndex()].mRightStick :
                                                mRStick2) :
                                           controller2->mRightStick;
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
    if (ImGui::DragFloat("Position", &mPos.y, 1, 200, 900, "%.1f"))
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
        if (ImGui::DragFloat("Position P2", &mPos2.y, 1, 200, 900, "%.1f"))
            saveData->queueSaveWrite();
        ImGui::PopID();
        ImGui::PopItemWidth();
    }

    return true;
}

#include "program/devgui/windows/input/WindowInput.h"

#include "al/Pad/NpadController.h"
#include "al/util/ControllerUtil.h"
#include "controller/seadControllerMgr.h"
#include "devgui/windows/WindowBase.h"
#include "helpers/GetHelper.h"
#include "helpers/PlayerHelper.h"
#include "helpers/InputHelper.h"
#include "game/Player/PlayerFunction.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "logger/Logger.hpp"
#include "game/System/GameDataFunction.h"
#include "al/util.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "nn/hid.h"

WindowInput::WindowInput(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault)
{
}

using InputCallback = bool;



void WindowInput::drawButton(
    const ImVec2& pos, InputCallback callback, float radius, ImU32 color, ImU32 pressedColor
) {
    const ImU32 col = callback ? pressedColor : color;
    ImGui::GetForegroundDrawList()->AddCircleFilled(pos, radius, col, 16);
}

void WindowInput::drawButtonRect(
    const ImVec2& pos, InputCallback callback, ImU32 color,ImU32 pressedColor
) {
    const ImU32 col = callback ? pressedColor : color;
    const ImVec2 min(pos.x - 13, pos.y - 6);
    const ImVec2 max(pos.x + 13, pos.y + 6);
    ImGui::GetForegroundDrawList()->AddRectFilled(min, max, col, 10.0f);
}

void WindowInput::drawInputDisplay() {

    if (!mIsEnabled) {
        return;
    }

    ImVec2 pos = mPos;
    pos.x -= 200;
    pos.y -= 100;

    if (true) {
        ImVec4 color = getInputDisplayColor(InputDisplayColor::Black);
        color.w = 128;
        ImGui::GetForegroundDrawList()->AddRectFilled({ pos.x - 50, pos.y - 100 }, { pos.x + 200, pos.y + 100 }, makeColor(color), 20.0f);
    }

    if (mIs2P) ImGui::GetForegroundDrawList()->AddText(ImGui::GetDefaultFont(), 30.f, {pos.x+20, pos.y-100}, makeColor(getInputDisplayColor(InputDisplayColor::White)), "Player 1");

    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 leftPos = { pos.x + controller->mLeftStick.x * 30, pos.y - controller->mLeftStick.y * 30 }; //stickl
    drawButton(leftPos, controller->mPadHold.isOnBit(controller->cPadIdx_1), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y += 30;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_Up)); //up
    pos.y += 30;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_Down)); //down
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_Left)); //left
    pos.x += 30;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_Right)); //right

    pos.x += 60;
    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 rightPos = { pos.x + controller->mRightStick.x * 30, pos.y - controller->mRightStick.y * 30 }; //stickr
    drawButton(rightPos, controller->mPadHold.isOnBit(controller->cPadIdx_2), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y -= 60;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_X)); //x
    pos.y += 30;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_B)); //b
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_Y)); //y
    pos.x += 30;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_A)); //a 

    pos.y -= 10;
    pos.x -= 75;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_Plus), 5); //plus
    pos.x -= 40;
    drawButton(pos, controller->mPadHold.isOnBit(controller->cPadIdx_Minus), 5); //minus

    pos = mPos;
    pos.x -= 200;
    pos.y -= 160;
    drawButtonRect(pos, controller->mPadHold.isOnBit(controller->cPadIdx_L)); //l
    pos.y -= 16;
    drawButtonRect(pos, controller->mPadHold.isOnBit(controller->cPadIdx_ZL)); //zl
    pos.x += 155;
    drawButtonRect(pos, controller->mPadHold.isOnBit(controller->cPadIdx_ZR)); //zr 
    pos.y += 16;
    drawButtonRect(pos, controller->mPadHold.isOnBit(controller->cPadIdx_R)); //r
}

void WindowInput::drawInputDisplayP2() {
    
    if (!mIsEnabled || !mIs2P) {
        return;
    }

    ImVec2 pos = mPos2;
    pos.x -= 200;
    pos.y -= 100;

    if (true) {
        ImVec4 color = getInputDisplayColor(InputDisplayColor::Black);
        color.w = 128;
        ImGui::GetForegroundDrawList()->AddRectFilled({ pos.x - 50, pos.y - 100 }, { pos.x + 200, pos.y + 100 }, makeColor(color), 20.0f);
    }
    ImGui::GetForegroundDrawList()->AddText(ImGui::GetDefaultFont(), 30.f, {pos.x+20, pos.y-100}, makeColor(getInputDisplayColor(InputDisplayColor::White)), "Player 2");

    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 leftPos = { pos.x + controller2->mLeftStick.x * 30, pos.y - controller2->mLeftStick.y * 30 }; //stickl
    drawButton(leftPos, controller2->mPadHold.isOnBit(controller->cPadIdx_1), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y += 30;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_Up)); //up
    pos.y += 30;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_Down)); //down
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_Left)); //left
    pos.x += 30;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_Right)); //right

    pos.x += 60;
    ImGui::GetForegroundDrawList()->AddCircle(pos, 25, makeColor(getInputDisplayColor(InputDisplayColor::Gray)), 0, 2);
    ImVec2 rightPos = { pos.x + controller2->mRightStick.x * 30, pos.y - controller2->mRightStick.y * 30 }; //stickr
    drawButton(rightPos, controller2->mPadHold.isOnBit(controller->cPadIdx_2), 16, makeColor(getInputDisplayColor(InputDisplayColor::White)));

    pos.x += 40;
    pos.y -= 60;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_X)); //x
    pos.y += 30;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_B)); //b
    pos.x -= 15;
    pos.y -= 15;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_Y)); //y
    pos.x += 30;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_A)); //a 

    pos.y -= 10;
    pos.x -= 75;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_Plus), 5); //plus
    pos.x -= 40;
    drawButton(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_Minus), 5); //minus

    pos = mPos2;
    pos.x -= 200;
    pos.y -= 160;
    drawButtonRect(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_L)); //l
    pos.y -= 16;
    drawButtonRect(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_ZL)); //zl
    pos.x += 155;
    drawButtonRect(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_ZR)); //zr 
    pos.y += 16;
    drawButtonRect(pos, controller2->mPadHold.isOnBit(controller2->cPadIdx_R)); //r
}


void WindowInput::updateWin()
{

    if (!mIsEnabled)
        return;
    controllerMgr = sead::ControllerMgr::instance();
    controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(0));
    if (mIs2P)
        controller2 = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(1));
}
bool WindowInput::tryUpdateWinDisplay()
{
    if(!WindowBase::tryUpdateWinDisplay())
        return false;

    ImGui::DragFloat2("Position", &mPos.x, 1, 200, 1600, "%.1f");
    ImGui::Checkbox("Enable", &mIsEnabled);
    ImGui::Checkbox("Enable P2", &mIs2P);
    if (mIs2P) {
        ImGui::DragFloat2("Position P2", &mPos2.x, 1, 200, 1600, "%.1f");
    }

    return true;

}
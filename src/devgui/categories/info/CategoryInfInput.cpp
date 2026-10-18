#include "devgui/categories/info/CategoryInfInput.h"

#include "sead/controller/seadControllerMgr.h"

#include "al/Library/Controller/InputFunction.h"
#include "al/Library/Controller/JoyPadAccelerometerAddon.h"
#include "al/Library/Controller/NpadController.h"
#include "al/Library/Controller/PadGyroAddon.h"

#include <cstdio>

#include "imgui.h"
#define DEG(X) X * 180 / M_PI

static void drawButton(const char* name, sead::Controller* controller, sead::Controller::PadIdx padIdx) {
    if (controller->mPadTrig.isOnBit(padIdx))
        ImGui::Text("%s Button: %s (Triggered)", name, controller->mPadHold.isOnBit(padIdx) ? "true" : "false");
    else if (controller->mPadRelease.isOnBit(padIdx))
        ImGui::Text("%s Button: %s (Released)", name, controller->mPadHold.isOnBit(padIdx) ? "true" : "false");
    else
        ImGui::Text("%s Button: %s", name, controller->mPadHold.isOnBit(padIdx) ? "true" : "false");
}

CategoryInfInput::CategoryInfInput(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryInfInput::updateCatDisplay() {
    for (int player = 0; player < 2; player++) {
        sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
        auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(player));

        auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 0);
        auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 1);
        auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 0);
        auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 1);

        snprintf(textBuffer, sizeof(textBuffer), "Controller #%i (%s):", player, controller->isConnected() ? "connected" : "disconnected");
        if (ImGui::CollapsingHeader(textBuffer)) {
            float leftRadius = controller->mLeftStick.length();
            float leftAngle = DEG(atan2f(controller->mLeftStick.x, controller->mLeftStick.y));
            ImGui::Text("Left Stick: X: %.3f Y: %.3f (Radius: %.3f Angle: %.3f)", controller->mLeftStick.x, controller->mLeftStick.y, leftRadius,
                        leftAngle);

            float rightRadius = controller->mRightStick.length();
            float rightAngle = DEG(atan2f(controller->mRightStick.x, controller->mRightStick.y));
            ImGui::Text("Right Stick: X: %.3f Y: %.3f (Radius: %.3f Angle: %.3f)", controller->mRightStick.x, controller->mRightStick.y, rightRadius,
                        rightAngle);

            ImGui::Separator();

            ImGui::Text("Left Accelerometer X: %.3f", accelLeft->mAcceleration.x);
            ImGui::Text("Left Accelerometer Y: %.3f", accelLeft->mAcceleration.y);
            ImGui::Text("Left Accelerometer Z: %.3f", accelLeft->mAcceleration.z);

            ImGui::Text("Right Accelerometer X: %.3f", accelRight->mAcceleration.x);
            ImGui::Text("Right Accelerometer Y: %.3f", accelRight->mAcceleration.y);
            ImGui::Text("Right Accelerometer Z: %.3f", accelRight->mAcceleration.z);

            ImGui::Separator();

            ImGui::Text("Left Gyroscope X: Angle: %.3f Velocity: %.3f", gyroLeft->mAngle.x, gyroLeft->mAngularVelocity.x);
            ImGui::Text("Left Gyroscope Y: Angle: %.3f Velocity: %.3f", gyroLeft->mAngle.y, gyroLeft->mAngularVelocity.y);
            ImGui::Text("Left Gyroscope Z: Angle: %.3f Velocity: %.3f", gyroLeft->mAngle.z, gyroLeft->mAngularVelocity.z);

            ImGui::Text("Right Gyroscope X: Angle: %.3f Velocity: %.3f", gyroRight->mAngle.x, gyroRight->mAngularVelocity.x);
            ImGui::Text("Right Gyroscope Y: Angle: %.3f Velocity: %.3f", gyroRight->mAngle.y, gyroRight->mAngularVelocity.y);
            ImGui::Text("Right Gyroscope Z: Angle: %.3f Velocity: %.3f", gyroRight->mAngle.z, gyroRight->mAngularVelocity.z);

            ImGui::Separator();

            drawButton("A", controller, sead::Controller::PadIdx::cPadIdx_A);
            drawButton("B", controller, sead::Controller::PadIdx::cPadIdx_B);
            drawButton("X", controller, sead::Controller::PadIdx::cPadIdx_X);
            drawButton("Y", controller, sead::Controller::PadIdx::cPadIdx_Y);
            drawButton("Left-Stick Press", controller, sead::Controller::PadIdx::cPadIdx_1);
            drawButton("Right-Stick Press", controller, sead::Controller::PadIdx::cPadIdx_2);
            drawButton("Home", controller, sead::Controller::PadIdx::cPadIdx_Home);
            drawButton("Touch", controller, sead::Controller::PadIdx::cPadIdx_Touch);
            drawButton("Plus", controller, sead::Controller::PadIdx::cPadIdx_Plus);
            drawButton("Minus", controller, sead::Controller::PadIdx::cPadIdx_Minus);
            drawButton("Start", controller, sead::Controller::PadIdx::cPadIdx_Start);
            drawButton("Select", controller, sead::Controller::PadIdx::cPadIdx_Select);
            drawButton("L", controller, sead::Controller::PadIdx::cPadIdx_L);
            drawButton("R", controller, sead::Controller::PadIdx::cPadIdx_R);
            drawButton("ZL", controller, sead::Controller::PadIdx::cPadIdx_ZL);
            drawButton("ZR", controller, sead::Controller::PadIdx::cPadIdx_ZR);
            drawButton("D-Pad Up", controller, sead::Controller::PadIdx::cPadIdx_Up);
            drawButton("D-Pad Down", controller, sead::Controller::PadIdx::cPadIdx_Down);
            drawButton("D-Pad Left", controller, sead::Controller::PadIdx::cPadIdx_Left);
            drawButton("D-Pad Right", controller, sead::Controller::PadIdx::cPadIdx_Right);
            drawButton("Left-Stick Up", controller, sead::Controller::PadIdx::cPadIdx_LeftStickUp);
            drawButton("Left-Stick Down", controller, sead::Controller::PadIdx::cPadIdx_LeftStickDown);
            drawButton("Left-Stick Left", controller, sead::Controller::PadIdx::cPadIdx_LeftStickLeft);
            drawButton("Left-Stick Right", controller, sead::Controller::PadIdx::cPadIdx_LeftStickRight);
            drawButton("Right-Stick Up", controller, sead::Controller::PadIdx::cPadIdx_RightStickUp);
            drawButton("Right-Stick Down", controller, sead::Controller::PadIdx::cPadIdx_RightStickDown);
            drawButton("Right-Stick Left", controller, sead::Controller::PadIdx::cPadIdx_RightStickLeft);
            drawButton("Right-Stick Right", controller, sead::Controller::PadIdx::cPadIdx_RightStickRight);
        }
    }

    return;
}

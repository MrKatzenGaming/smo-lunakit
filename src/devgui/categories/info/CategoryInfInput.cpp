#include "devgui/categories/info/CategoryInfInput.h"

#include "nn/hid.h"

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

    if (ImGui::CollapsingHeader("Keyboard")) {
        nn::hid::KeyboardState keyboardState;
        nn::hid::GetKeyboardState(&keyboardState);

        ImGui::Text("Sampling Number: %lu", keyboardState.mSamplingNumber);

        ImGui::SeparatorText("Modifiers");

        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::Control)))
            ImGui::BulletText("Control");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::Shift)))
            ImGui::BulletText("Shift");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::LeftAlt)))
            ImGui::BulletText("LeftAlt");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::RightAlt)))
            ImGui::BulletText("RightAlt");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::Gui)))
            ImGui::BulletText("Gui");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::CapsLock)))
            ImGui::BulletText("CapsLock");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::ScrollLock)))
            ImGui::BulletText("ScrollLock");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::NumLock)))
            ImGui::BulletText("NumLock");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::Katakana)))
            ImGui::BulletText("Katakana");
        if (keyboardState.mModifiers.Test(static_cast<u64>(nn::hid::KeyboardModifier::Hiragana)))
            ImGui::BulletText("Hiragana");

        ImGui::SeparatorText("Keys");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::A)))
            ImGui::BulletText("A");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::B)))
            ImGui::BulletText("B");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::C)))
            ImGui::BulletText("C");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D)))
            ImGui::BulletText("D");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::E)))
            ImGui::BulletText("E");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F)))
            ImGui::BulletText("F");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::G)))
            ImGui::BulletText("G");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::H)))
            ImGui::BulletText("H");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::I)))
            ImGui::BulletText("I");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::J)))
            ImGui::BulletText("J");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::K)))
            ImGui::BulletText("K");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::L)))
            ImGui::BulletText("L");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::M)))
            ImGui::BulletText("M");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::N)))
            ImGui::BulletText("N");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::O)))
            ImGui::BulletText("O");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::P)))
            ImGui::BulletText("P");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Q)))
            ImGui::BulletText("Q");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::R)))
            ImGui::BulletText("R");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::S)))
            ImGui::BulletText("S");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::T)))
            ImGui::BulletText("T");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::U)))
            ImGui::BulletText("U");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::V)))
            ImGui::BulletText("V");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::W)))
            ImGui::BulletText("W");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::X)))
            ImGui::BulletText("X");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Y)))
            ImGui::BulletText("Y");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Z)))
            ImGui::BulletText("Z");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D1)))
            ImGui::BulletText("D1");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D2)))
            ImGui::BulletText("D2");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D3)))
            ImGui::BulletText("D3");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D4)))
            ImGui::BulletText("D4");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D5)))
            ImGui::BulletText("D5");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D6)))
            ImGui::BulletText("D6");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D7)))
            ImGui::BulletText("D7");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D8)))
            ImGui::BulletText("D8");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D9)))
            ImGui::BulletText("D9");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::D0)))
            ImGui::BulletText("D0");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Return)))
            ImGui::BulletText("Return");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Escape)))
            ImGui::BulletText("Escape");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Backspace)))
            ImGui::BulletText("Backspace");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Tab)))
            ImGui::BulletText("Tab");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Space)))
            ImGui::BulletText("Space");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Minus)))
            ImGui::BulletText("Minus");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Plus)))
            ImGui::BulletText("Plus");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::OpenBracket)))
            ImGui::BulletText("OpenBracket");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::CloseBracket)))
            ImGui::BulletText("CloseBracket");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Pipe)))
            ImGui::BulletText("Pipe");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Tilde)))
            ImGui::BulletText("Tilde");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Semicolon)))
            ImGui::BulletText("Semicolon");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Quote)))
            ImGui::BulletText("Quote");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Backquote)))
            ImGui::BulletText("Backquote");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Comma)))
            ImGui::BulletText("Comma");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Period)))
            ImGui::BulletText("Period");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Slash)))
            ImGui::BulletText("Slash");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::CapsLock)))
            ImGui::BulletText("CapsLock");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F1)))
            ImGui::BulletText("F1");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F2)))
            ImGui::BulletText("F2");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F3)))
            ImGui::BulletText("F3");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F4)))
            ImGui::BulletText("F4");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F5)))
            ImGui::BulletText("F5");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F6)))
            ImGui::BulletText("F6");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F7)))
            ImGui::BulletText("F7");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F8)))
            ImGui::BulletText("F8");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F9)))
            ImGui::BulletText("F9");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F10)))
            ImGui::BulletText("F10");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F11)))
            ImGui::BulletText("F11");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F12)))
            ImGui::BulletText("F12");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::PrintScreen)))
            ImGui::BulletText("PrintScreen");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::ScrollLock)))
            ImGui::BulletText("ScrollLock");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Pause)))
            ImGui::BulletText("Pause");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Insert)))
            ImGui::BulletText("Insert");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Home)))
            ImGui::BulletText("Home");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::PageUp)))
            ImGui::BulletText("PageUp");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Delete)))
            ImGui::BulletText("Delete");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::End)))
            ImGui::BulletText("End");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::PageDown)))
            ImGui::BulletText("PageDown");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::RightArrow)))
            ImGui::BulletText("RightArrow");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::LeftArrow)))
            ImGui::BulletText("LeftArrow");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::DownArrow)))
            ImGui::BulletText("DownArrow");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::UpArrow)))
            ImGui::BulletText("UpArrow");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumLock)))
            ImGui::BulletText("NumLock");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadDivide)))
            ImGui::BulletText("NumPadDivide");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadMultiply)))
            ImGui::BulletText("NumPadMultiply");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadSubtract)))
            ImGui::BulletText("NumPadSubtract");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadAdd)))
            ImGui::BulletText("NumPadAdd");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadEnter)))
            ImGui::BulletText("NumPadEnter");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad1)))
            ImGui::BulletText("NumPad1");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad2)))
            ImGui::BulletText("NumPad2");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad3)))
            ImGui::BulletText("NumPad3");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad4)))
            ImGui::BulletText("NumPad4");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad5)))
            ImGui::BulletText("NumPad5");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad6)))
            ImGui::BulletText("NumPad6");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad7)))
            ImGui::BulletText("NumPad7");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad8)))
            ImGui::BulletText("NumPad8");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad9)))
            ImGui::BulletText("NumPad9");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPad0)))
            ImGui::BulletText("NumPad0");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadDot)))
            ImGui::BulletText("NumPadDot");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Backslash)))
            ImGui::BulletText("Backslash");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Application)))
            ImGui::BulletText("Application");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Power)))
            ImGui::BulletText("Power");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadEquals)))
            ImGui::BulletText("NumPadEquals");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F13)))
            ImGui::BulletText("F13");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F14)))
            ImGui::BulletText("F14");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F15)))
            ImGui::BulletText("F15");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F16)))
            ImGui::BulletText("F16");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F17)))
            ImGui::BulletText("F17");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F18)))
            ImGui::BulletText("F18");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F19)))
            ImGui::BulletText("F19");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F20)))
            ImGui::BulletText("F20");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F21)))
            ImGui::BulletText("F21");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F22)))
            ImGui::BulletText("F22");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F23)))
            ImGui::BulletText("F23");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::F24)))
            ImGui::BulletText("F24");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadComma)))
            ImGui::BulletText("NumPadComma");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Ro)))
            ImGui::BulletText("Ro");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::KatakanaHiragana)))
            ImGui::BulletText("KatakanaHiragana");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Yen)))
            ImGui::BulletText("Yen");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Henkan)))
            ImGui::BulletText("Henkan");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Muhenkan)))
            ImGui::BulletText("Muhenkan");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::NumPadCommaPc98)))
            ImGui::BulletText("NumPadCommaPc98");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::HangulEnglish)))
            ImGui::BulletText("HangulEnglish");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Hanja)))
            ImGui::BulletText("Hanja");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Katakana)))
            ImGui::BulletText("Katakana");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::Hiragana)))
            ImGui::BulletText("Hiragana");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::ZenkakuHankaku)))
            ImGui::BulletText("ZenkakuHankaku");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::LeftControl)))
            ImGui::BulletText("LeftControl");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::LeftShift)))
            ImGui::BulletText("LeftShift");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::LeftAlt)))
            ImGui::BulletText("LeftAlt");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::LeftGui)))
            ImGui::BulletText("LeftGui");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::RightControl)))
            ImGui::BulletText("RightControl");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::RightShift)))
            ImGui::BulletText("RightShift");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::RightAlt)))
            ImGui::BulletText("RightAlt");
        if (keyboardState.mKeys.Test(static_cast<u64>(nn::hid::KeyboardKey::RightGui)))
            ImGui::BulletText("RightGui");
    }

    if (ImGui::CollapsingHeader("Mouse")) {
        nn::hid::MouseState mouseState;
        nn::hid::GetMouseState(&mouseState);

        ImGui::Text("Sampling Number: %lu", mouseState.mSamplingNumber);

        ImGui::SeparatorText("Location");
        ImGui::Text("Position: X: %d Y: %d", mouseState.mX, mouseState.mY);
        ImGui::Text("Delta: X: %d Y: %d", mouseState.mDeltaX, mouseState.mDeltaY);
        ImGui::Text("Wheel Delta: X: %d Y: %d", mouseState.mWheelDeltaX, mouseState.mWheelDeltaY);

        ImGui::SeparatorText("Attributes");
        if (mouseState.mAttributes.Test(static_cast<u64>(nn::hid::MouseAttribute::Transferable)))
            ImGui::BulletText("Transferable");
        if (mouseState.mAttributes.Test(static_cast<u64>(nn::hid::MouseAttribute::IsConnected)))
            ImGui::BulletText("IsConnected");

        ImGui::SeparatorText("Buttons");
        if (mouseState.mButtons.Test(static_cast<u64>(nn::hid::MouseButton::Left)))
            ImGui::BulletText("Left");
        if (mouseState.mButtons.Test(static_cast<u64>(nn::hid::MouseButton::Right)))
            ImGui::BulletText("Right");
        if (mouseState.mButtons.Test(static_cast<u64>(nn::hid::MouseButton::Middle)))
            ImGui::BulletText("Middle");
        if (mouseState.mButtons.Test(static_cast<u64>(nn::hid::MouseButton::Forward)))
            ImGui::BulletText("Forward");
        if (mouseState.mButtons.Test(static_cast<u64>(nn::hid::MouseButton::Back)))
            ImGui::BulletText("Back");
    }

    return;
}

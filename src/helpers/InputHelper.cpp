#include "helpers/InputHelper.h"

#include "devgui/DevGuiManager.h"

static const char* styleNames[] = {
    "Pro Controller",
    "Joy-Con controller in handheld mode",
    "Joy-Con controller in dual mode",
    "Joy-Con left controller in single mode",
    "Joy-Con right controller in single mode",
    "GameCube controller",
    "Poké Ball Plus controller",
    "NES/Famicom controller",
    "NES/Famicom controller in handheld mode",
    "SNES controller",
    "N64 controller",
    "Sega Genesis controller",
    "generic external controller",
    "generic controller",
};

nn::hid::NpadBaseState InputHelper::prevControllerState{};
nn::hid::NpadBaseState InputHelper::curControllerState{};

nn::hid::KeyboardState InputHelper::curKeyboardState{};
nn::hid::KeyboardState InputHelper::prevKeyboardState{};

nn::hid::MouseState InputHelper::curMouseState{};
nn::hid::MouseState InputHelper::prevMouseState{};

ulong InputHelper::selectedPort = -1;
bool InputHelper::isReadInput = true;
bool InputHelper::toggleInput = false;
bool InputHelper::enableScroll = true;

void InputHelper::initKBM() {
    nn::hid::InitializeKeyboard();
    nn::hid::InitializeMouse();
}

void InputHelper::updatePadState() {
    setIsHandheldMode();

    prevControllerState = curControllerState;
    tryGetContState(&curControllerState, selectedPort);

    prevKeyboardState = curKeyboardState;
    nn::hid::GetKeyboardState(&curKeyboardState);

    prevMouseState = curMouseState;
    nn::hid::GetMouseState(&curMouseState);

    if (isHoldR() && isHoldZR() && isPressZL())
        toggleInput = !toggleInput;

    if (!DevGuiManager::instance()->isMenuActive())
        toggleInput = false;
}

bool InputHelper::tryGetContState(nn::hid::NpadBaseState* state, ulong port) {
    nn::hid::NpadStyleSet styleSet = nn::hid::GetNpadStyleSet(port);
    isReadInput = true;
    bool result = true;

    if (styleSet.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleFullKey)))
        nn::hid::GetNpadState((nn::hid::NpadFullKeyState*)state, port);
    else if (styleSet.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleHandheld)))
        nn::hid::GetNpadState((nn::hid::NpadHandheldState*)state, port);
    else if (styleSet.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleJoyDual)))
        nn::hid::GetNpadState((nn::hid::NpadJoyDualState*)state, port);
    else if (styleSet.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleJoyLeft)))
        nn::hid::GetNpadState((nn::hid::NpadJoyLeftState*)state, port);
    else if (styleSet.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleJoyRight)))
        nn::hid::GetNpadState((nn::hid::NpadJoyRightState*)state, port);
    else
        result = false;

    isReadInput = false;

    return result;
}

void InputHelper::setIsHandheldMode() {
    setPort(0);

    nn::hid::NpadStyleSet style = nn::hid::GetNpadStyleSet(0);  // Gets player 1's controller style
    // If no controller is connected in port 0, migrate selected port to handheld 0x20

    if (style.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleFullKey)))
        return;
    if (style.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleJoyDual)))
        return;
    if (style.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleJoyLeft)))
        return;
    if (style.Test(static_cast<u64>(nn::hid::NpadStyleTag::NpadStyleJoyRight)))
        return;

    setPort(0x20);
}

bool InputHelper::isButtonHold(nn::hid::NpadButton button) {
    return curControllerState.mButtons.Test(static_cast<u64>(button));
}

bool InputHelper::isButtonPress(nn::hid::NpadButton button) {
    return curControllerState.mButtons.Test(static_cast<u64>(button)) && !prevControllerState.mButtons.Test(static_cast<u64>(button));
}

bool InputHelper::isButtonRelease(nn::hid::NpadButton button) {
    return !curControllerState.mButtons.Test(static_cast<u64>(button)) && prevControllerState.mButtons.Test(static_cast<u64>(button));
}

bool InputHelper::isKeyHold(nn::hid::KeyboardKey key) {
    return curKeyboardState.mKeys.Test(static_cast<u64>(key));
}

bool InputHelper::isModifierHold(nn::hid::KeyboardModifier modifier) {
    return curKeyboardState.mModifiers.Test(static_cast<u64>(modifier));
}

bool InputHelper::isKeyPress(nn::hid::KeyboardKey key) {
    return curKeyboardState.mKeys.Test(static_cast<u64>(key)) && !prevKeyboardState.mKeys.Test(static_cast<u64>(key));
}

bool InputHelper::isKeyRelease(nn::hid::KeyboardKey key) {
    return !curKeyboardState.mKeys.Test(static_cast<u64>(key)) && prevKeyboardState.mKeys.Test(static_cast<u64>(key));
}

bool InputHelper::isMouseHold(nn::hid::MouseButton button) {
    return curMouseState.mButtons.Test(static_cast<u64>(button));
}

bool InputHelper::isMousePress(nn::hid::MouseButton button) {
    return curMouseState.mButtons.Test(static_cast<u64>(button)) && !prevMouseState.mButtons.Test(static_cast<u64>(button));
}

bool InputHelper::isMouseRelease(nn::hid::MouseButton button) {
    return !curMouseState.mButtons.Test(static_cast<u64>(button)) && prevMouseState.mButtons.Test(static_cast<u64>(button));
}

bool InputHelper::isMouseConnected() {
    return curMouseState.mAttributes.Test((int)nn::hid::MouseAttribute::IsConnected);
}

void InputHelper::getMouseCoords(float* x, float* y) {
    *x = curMouseState.mX;
    *y = curMouseState.mY;
}

void InputHelper::getScrollDelta(float* x, float* y) {
    if (InputHelper::canScroll()) {
        *x = curMouseState.mWheelDeltaX;
        *y = curMouseState.mWheelDeltaY;
    } else {
        *x = 0.f;
        *y = 0.f;
    }
}

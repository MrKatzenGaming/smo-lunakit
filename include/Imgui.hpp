#pragma once
#include "hk/gfx/ImGuiBackendNvn.h"

#include "nn/hid.h"

#include "sead/heap/seadExpHeap.h"
#include "sead/heap/seadHeap.h"

#include <cstring>

#include "helpers/InputHelper.h"
#include "helpers/fsHelper.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace imgui {

static sead::Heap* sImGuiHeap = nullptr;

static void setupFont() {
    FsHelper::LoadData loadData = {.path = "sd:/LunaKit/ImGuiData/Font/SFMonoSquare-Regular.otf"};

    FsHelper::loadFileFromPath(loadData);

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesDefault());
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
    builder.AddText(""
                    "！、"
                    "äüöß"
                    "!\"§$%&/()=?´`^°#+-·.,;:_'*\\}][{<>|"
                    "←→↓↑");

    builder.BuildRanges(&ranges);

    ImFontConfig c{};
    strncpy(c.Name, "SFMonoSquare", sizeof(c.Name) - 1);
    ImFont* font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(loadData.buffer, loadData.bufSize, 17, &c, ranges.Data);

    hk::gfx::ImGuiBackendNvn::instance()->initTexture(true);
    ImGui::GetIO().FontDefault = font;
}

struct KeyMapping {
    ImGuiKey imguiKey;
    nn::hid::KeyboardKey hidKey;
    char lowerChar;
    char upperChar;
};
struct NPadMapping {
    ImGuiKey imguiKey;
    nn::hid::NpadButton hidKey;
};

static void updateImGuiInput() {
    static nn::hid::MouseState mouseState{};
    static nn::hid::MouseState lastMouseState{};
    static nn::hid::KeyboardState keyboardState{};
    static nn::hid::KeyboardState lastKeyboardState{};

    lastMouseState = mouseState;
    lastKeyboardState = keyboardState;

    nn::hid::GetMouseState(&mouseState);
    nn::hid::GetKeyboardState(&keyboardState);

    ImGuiIO& io = ImGui::GetIO();

    // Mouse position
    io.AddMousePosEvent(mouseState.mX / 1280.f * io.DisplaySize.x, mouseState.mY / 720.f * io.DisplaySize.y);

    // Mouse buttons
    constexpr std::pair<nn::hid::MouseButton, ImGuiMouseButton> buttonMap[] = {{nn::hid::MouseButton::Left, ImGuiMouseButton_Left},
                                                                               {nn::hid::MouseButton::Right, ImGuiMouseButton_Right},
                                                                               {nn::hid::MouseButton::Middle, ImGuiMouseButton_Middle}};

    for (auto&& [hidBtn, imguiBtn] : buttonMap)
        if (InputHelper::isMousePress((nn::hid::MouseButton)hidBtn))
            io.AddMouseButtonEvent((ImGuiMouseButton)imguiBtn, true);
        else if (InputHelper::isMouseRelease((nn::hid::MouseButton)hidBtn))
            io.AddMouseButtonEvent((ImGuiMouseButton)imguiBtn, false);

    // Mouse wheel
    ImVec2 scrollDelta(0, 0);
    InputHelper::getScrollDelta(&scrollDelta.x, &scrollDelta.y);
    io.AddMouseWheelEvent(scrollDelta.x * 0.5f, scrollDelta.y * 0.5f);

    // ----------------------------
    // Keyboard → ImGui
    // ----------------------------

    constexpr KeyMapping key_mappings[] = {
        {ImGuiKey_Tab, nn::hid::KeyboardKey::Tab, '\t', '\t'},
        {ImGuiKey_LeftArrow, nn::hid::KeyboardKey::LeftArrow, '\0', '\0'},
        {ImGuiKey_RightArrow, nn::hid::KeyboardKey::RightArrow, '\0', '\0'},
        {ImGuiKey_UpArrow, nn::hid::KeyboardKey::UpArrow, '\0', '\0'},
        {ImGuiKey_DownArrow, nn::hid::KeyboardKey::DownArrow, '\0', '\0'},
        {ImGuiKey_PageUp, nn::hid::KeyboardKey::PageUp, '\0', '\0'},
        {ImGuiKey_PageDown, nn::hid::KeyboardKey::PageDown, '\0', '\0'},
        {ImGuiKey_Home, nn::hid::KeyboardKey::Home, '\0', '\0'},
        {ImGuiKey_End, nn::hid::KeyboardKey::End, '\0', '\0'},
        {ImGuiKey_Insert, nn::hid::KeyboardKey::Insert, '\0', '\0'},
        {ImGuiKey_Delete, nn::hid::KeyboardKey::Delete, '\0', '\0'},
        {ImGuiKey_Backspace, nn::hid::KeyboardKey::Backspace, '\0', '\0'},
        {ImGuiKey_Space, nn::hid::KeyboardKey::Space, ' ', ' '},
        {ImGuiKey_Enter, nn::hid::KeyboardKey::Return, '\0', '\0'},
        {ImGuiKey_Escape, nn::hid::KeyboardKey::Escape, '\0', '\0'},
        {ImGuiKey_LeftCtrl, nn::hid::KeyboardKey::LeftControl, '\0', '\0'},
        {ImGuiKey_LeftShift, nn::hid::KeyboardKey::LeftShift, '\0', '\0'},
        {ImGuiKey_LeftAlt, nn::hid::KeyboardKey::LeftAlt, '\0', '\0'},
        {ImGuiKey_LeftSuper, nn::hid::KeyboardKey::LeftGui, '\0', '\0'},
        {ImGuiKey_RightCtrl, nn::hid::KeyboardKey::RightControl, '\0', '\0'},
        {ImGuiKey_RightShift, nn::hid::KeyboardKey::RightShift, '\0', '\0'},
        {ImGuiKey_RightAlt, nn::hid::KeyboardKey::RightAlt, '\0', '\0'},
        {ImGuiKey_RightSuper, nn::hid::KeyboardKey::RightGui, '\0', '\0'},
        {ImGuiKey_Menu, nn::hid::KeyboardKey::Application, '\0', '\0'},  // not positive about this
        {ImGuiKey_0, nn::hid::KeyboardKey::D0, '0', ')'},
        {ImGuiKey_1, nn::hid::KeyboardKey::D1, '1', '!'},
        {ImGuiKey_2, nn::hid::KeyboardKey::D2, '2', '@'},
        {ImGuiKey_3, nn::hid::KeyboardKey::D3, '3', '#'},
        {ImGuiKey_4, nn::hid::KeyboardKey::D4, '4', '$'},
        {ImGuiKey_5, nn::hid::KeyboardKey::D5, '5', '%'},
        {ImGuiKey_6, nn::hid::KeyboardKey::D6, '6', '^'},
        {ImGuiKey_7, nn::hid::KeyboardKey::D7, '7', '&'},
        {ImGuiKey_8, nn::hid::KeyboardKey::D8, '8', '*'},
        {ImGuiKey_9, nn::hid::KeyboardKey::D9, '9', '('},
        {ImGuiKey_A, nn::hid::KeyboardKey::A, 'a', 'A'},
        {ImGuiKey_B, nn::hid::KeyboardKey::B, 'b', 'B'},
        {ImGuiKey_C, nn::hid::KeyboardKey::C, 'c', 'C'},
        {ImGuiKey_D, nn::hid::KeyboardKey::D, 'd', 'D'},
        {ImGuiKey_E, nn::hid::KeyboardKey::E, 'e', 'E'},
        {ImGuiKey_F, nn::hid::KeyboardKey::F, 'f', 'F'},
        {ImGuiKey_G, nn::hid::KeyboardKey::G, 'g', 'G'},
        {ImGuiKey_H, nn::hid::KeyboardKey::H, 'h', 'H'},
        {ImGuiKey_I, nn::hid::KeyboardKey::I, 'i', 'I'},
        {ImGuiKey_J, nn::hid::KeyboardKey::J, 'j', 'J'},
        {ImGuiKey_K, nn::hid::KeyboardKey::K, 'k', 'K'},
        {ImGuiKey_L, nn::hid::KeyboardKey::L, 'l', 'L'},
        {ImGuiKey_M, nn::hid::KeyboardKey::M, 'm', 'M'},
        {ImGuiKey_N, nn::hid::KeyboardKey::N, 'n', 'N'},
        {ImGuiKey_O, nn::hid::KeyboardKey::O, 'o', 'O'},
        {ImGuiKey_P, nn::hid::KeyboardKey::P, 'p', 'P'},
        {ImGuiKey_Q, nn::hid::KeyboardKey::Q, 'q', 'Q'},
        {ImGuiKey_R, nn::hid::KeyboardKey::R, 'r', 'R'},
        {ImGuiKey_S, nn::hid::KeyboardKey::S, 's', 'S'},
        {ImGuiKey_T, nn::hid::KeyboardKey::T, 't', 'T'},
        {ImGuiKey_U, nn::hid::KeyboardKey::U, 'u', 'U'},
        {ImGuiKey_V, nn::hid::KeyboardKey::V, 'v', 'V'},
        {ImGuiKey_W, nn::hid::KeyboardKey::W, 'w', 'W'},
        {ImGuiKey_X, nn::hid::KeyboardKey::X, 'x', 'X'},
        {ImGuiKey_Y, nn::hid::KeyboardKey::Y, 'y', 'Y'},
        {ImGuiKey_Z, nn::hid::KeyboardKey::Z, 'z', 'Z'},
        {ImGuiKey_F1, nn::hid::KeyboardKey::F1, '\0', '\0'},
        {ImGuiKey_F2, nn::hid::KeyboardKey::F2, '\0', '\0'},
        {ImGuiKey_F3, nn::hid::KeyboardKey::F3, '\0', '\0'},
        {ImGuiKey_F4, nn::hid::KeyboardKey::F4, '\0', '\0'},
        {ImGuiKey_F5, nn::hid::KeyboardKey::F5, '\0', '\0'},
        {ImGuiKey_F6, nn::hid::KeyboardKey::F6, '\0', '\0'},
        {ImGuiKey_F7, nn::hid::KeyboardKey::F7, '\0', '\0'},
        {ImGuiKey_F8, nn::hid::KeyboardKey::F8, '\0', '\0'},
        {ImGuiKey_F9, nn::hid::KeyboardKey::F9, '\0', '\0'},
        {ImGuiKey_F10, nn::hid::KeyboardKey::F10, '\0', '\0'},
        {ImGuiKey_F11, nn::hid::KeyboardKey::F11, '\0', '\0'},
        {ImGuiKey_F12, nn::hid::KeyboardKey::F12, '\0', '\0'},
        {ImGuiKey_Apostrophe, nn::hid::KeyboardKey::Quote, '\'', '\"'},
        {ImGuiKey_Comma, nn::hid::KeyboardKey::Comma, ',', '<'},
        {ImGuiKey_Minus, nn::hid::KeyboardKey::Minus, '-', '_'},
        {ImGuiKey_Period, nn::hid::KeyboardKey::Period, '.', '>'},
        {ImGuiKey_Slash, nn::hid::KeyboardKey::Slash, '/', '?'},
        {ImGuiKey_Semicolon, nn::hid::KeyboardKey::Semicolon, ';', ':'},
        {ImGuiKey_Equal, nn::hid::KeyboardKey::Plus, '=', '+'},
        {ImGuiKey_LeftBracket, nn::hid::KeyboardKey::OpenBracket, '[', '{'},
        {ImGuiKey_Backslash, nn::hid::KeyboardKey::Backslash, '\\', '|'},
        {ImGuiKey_RightBracket, nn::hid::KeyboardKey::CloseBracket, ']', '}'},
        {ImGuiKey_GraveAccent, nn::hid::KeyboardKey::Backquote, '`', '~'},
        {ImGuiKey_CapsLock, nn::hid::KeyboardKey::CapsLock, '\0', '\0'},
        {ImGuiKey_ScrollLock, nn::hid::KeyboardKey::ScrollLock, '\0', '\0'},
        {ImGuiKey_NumLock, nn::hid::KeyboardKey::NumLock, '\0', '\0'},
        {ImGuiKey_PrintScreen, nn::hid::KeyboardKey::PrintScreen, '\0', '\0'},
        {ImGuiKey_Pause, nn::hid::KeyboardKey::Pause, '\0', '\0'},
        {ImGuiKey_Keypad0, nn::hid::KeyboardKey::NumPad0, '0', '0'},
        {ImGuiKey_Keypad1, nn::hid::KeyboardKey::NumPad1, '1', '1'},
        {ImGuiKey_Keypad2, nn::hid::KeyboardKey::NumPad2, '2', '2'},
        {ImGuiKey_Keypad3, nn::hid::KeyboardKey::NumPad3, '3', '3'},
        {ImGuiKey_Keypad4, nn::hid::KeyboardKey::NumPad4, '4', '4'},
        {ImGuiKey_Keypad5, nn::hid::KeyboardKey::NumPad5, '5', '5'},
        {ImGuiKey_Keypad6, nn::hid::KeyboardKey::NumPad6, '6', '6'},
        {ImGuiKey_Keypad7, nn::hid::KeyboardKey::NumPad7, '7', '7'},
        {ImGuiKey_Keypad8, nn::hid::KeyboardKey::NumPad8, '8', '8'},
        {ImGuiKey_Keypad9, nn::hid::KeyboardKey::NumPad9, '9', '9'},
        {ImGuiKey_KeypadDecimal, nn::hid::KeyboardKey::NumPadDot, '.', '.'},
        {ImGuiKey_KeypadDivide, nn::hid::KeyboardKey::NumPadDivide, '/', '/'},
        {ImGuiKey_KeypadMultiply, nn::hid::KeyboardKey::NumPadMultiply, '*', '*'},
        {ImGuiKey_KeypadSubtract, nn::hid::KeyboardKey::NumPadSubtract, '-', '-'},
        {ImGuiKey_KeypadAdd, nn::hid::KeyboardKey::NumPadAdd, '+', '+'},
        {ImGuiKey_KeypadEnter, nn::hid::KeyboardKey::NumPadEnter, '\n', '\n'},
        {ImGuiKey_KeypadEqual, nn::hid::KeyboardKey::NumPadEquals, '=', '='},
    };

    bool isShift = InputHelper::isModifierHold(nn::hid::KeyboardModifier::Shift) || InputHelper::isModifierHold(nn::hid::KeyboardModifier::CapsLock);
    bool isCtrl = InputHelper::isModifierHold(nn::hid::KeyboardModifier::Control);
    bool isAlt = InputHelper::isModifierHold(nn::hid::KeyboardModifier::LeftAlt) || InputHelper::isModifierHold(nn::hid::KeyboardModifier::RightAlt);

    for (auto& mapping : key_mappings) {
        if (InputHelper::isKeyPress(mapping.hidKey)) {
            io.AddKeyEvent(mapping.imguiKey, true);

            char character = isShift ? mapping.upperChar : mapping.lowerChar;
            if (character != '\0')
                io.AddInputCharacter(character);
        } else if (InputHelper::isKeyRelease(mapping.hidKey)) {
            io.AddKeyEvent(mapping.imguiKey, false);
        }
    }

    io.AddKeyEvent(ImGuiMod_Shift, isShift);
    io.AddKeyEvent(ImGuiMod_Ctrl, isCtrl);
    io.AddKeyEvent(ImGuiMod_Alt, isAlt);

    constexpr NPadMapping npad_mappings[] = {
        {ImGuiKey_GamepadFaceDown, nn::hid::NpadButton::A},
        {ImGuiKey_GamepadFaceRight, nn::hid::NpadButton::B},
        {ImGuiKey_GamepadFaceUp, nn::hid::NpadButton::X},
        {ImGuiKey_GamepadFaceLeft, nn::hid::NpadButton::Y},
        {ImGuiKey_GamepadL1, nn::hid::NpadButton::L},
        {ImGuiKey_GamepadR1, nn::hid::NpadButton::R},
        {ImGuiKey_GamepadL2, nn::hid::NpadButton::ZL},
        {ImGuiKey_GamepadR2, nn::hid::NpadButton::ZR},
        {ImGuiKey_GamepadStart, nn::hid::NpadButton::Plus},
        {ImGuiKey_GamepadBack, nn::hid::NpadButton::Minus},
        {ImGuiKey_GamepadDpadLeft, nn::hid::NpadButton::Left},
        {ImGuiKey_GamepadDpadRight, nn::hid::NpadButton::Right},
        {ImGuiKey_GamepadDpadUp, nn::hid::NpadButton::Up},
        {ImGuiKey_GamepadDpadDown, nn::hid::NpadButton::Down},
        {ImGuiKey_GamepadLStickLeft, nn::hid::NpadButton::StickLLeft},
        {ImGuiKey_GamepadLStickRight, nn::hid::NpadButton::StickLRight},
        {ImGuiKey_GamepadLStickUp, nn::hid::NpadButton::StickLUp},
        {ImGuiKey_GamepadLStickDown, nn::hid::NpadButton::StickLDown},
    };

    if (InputHelper::isInputToggled()) {
        for (auto& mapping : npad_mappings)
            if (InputHelper::isButtonPress(mapping.hidKey))
                io.AddKeyEvent(mapping.imguiKey, true);
            else if (InputHelper::isButtonRelease(mapping.hidKey))
                io.AddKeyEvent(mapping.imguiKey, false);
    }

    io.MouseDrawCursor = true;
}

static void updateScale(bool isDocked, float dockedScale, float handheldScale) {
    static float prevScale = 0.0f;

    float scale = isDocked ? dockedScale : handheldScale;

    ImGuiStyle& stylePtr = ImGui::GetStyle();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO& io = ImGui::GetIO();

    ImVec4 prevColors[ImGuiCol_COUNT] = {};

    for (int i = 0; i < ImGuiCol_COUNT; i++)
        prevColors[i] = stylePtr.Colors[i];

    float prevAlph = stylePtr.Alpha;

    // reset style
    stylePtr = ImGuiStyle();
    // set colors back to previous
    for (int i = 0; i < ImGuiCol_COUNT; i++)
        stylePtr.Colors[i] = prevColors[i];
    stylePtr.Alpha = prevAlph;
    // scale style
    ImGui::GetStyle().ScaleAllSizes(scale);
    // reset scale of windows
    if (prevScale != 0.0f)
        ImGui::ScaleWindowsInViewport((ImGuiViewportP*)viewport, 1.f / prevScale);

    // scale window
    ImGui::ScaleWindowsInViewport((ImGuiViewportP*)viewport, scale);
    prevScale = scale;
    // set font scale
    stylePtr.FontScaleMain = scale;
}

static void setup(sead::Heap* baseHeap) {
    sImGuiHeap = sead::ExpHeap::create(8_MB, "ImGuiHeap", baseHeap, 8, sead::Heap::cHeapDirection_Forward, true);

    hk::gfx::ImGuiBackendNvn* imgui = hk::gfx::ImGuiBackendNvn::instance();

    imgui->setAllocator({[](size allocSize, size alignment) -> void* { return sImGuiHeap->tryAlloc(allocSize, alignment); },
                         [](void* ptr) -> void { sImGuiHeap->free(ptr); }});

    imgui->tryInitialize();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.ConfigDebugHighlightIdConflicts = false;

    setupFont();
}

}  // namespace imgui

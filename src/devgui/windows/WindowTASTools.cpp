#include "devgui/windows/WindowTASTools.h"

#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"

#include "al/Library/Math/MathUtil.h"

#include "game/Player/PlayerActorHakoniwa.h"

#include "custom/game/Player/HackCap.h"

#include <cstdint>

#include "devgui/DevGuiHooks.h"
#include "devgui/DevGuiManager.h"
#include "devgui/windows/WindowBase.h"
#include "helpers/ImGuiHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "smo-tas/STAS.h"

WindowTASTools::WindowTASTools(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {}

bool WindowTASTools::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    ImGui::Combo("Wiggler Pattern", &mCurPattern, patterns, hk::util::arraySize(patterns));

    // ImGui::Text("Current Pattern: %s", patterns[mCurPattern]);
    // ImGui::Text("Current Pattern ID: %d", mCurPattern);
    auto* tas = STAS::instance();
    if (!tas)
        return false;

    ImGui::Checkbox("Absolute Joystick", &tas->mIsUseAbsJoystick);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Can be toggled using L-Stick while a TAS is running");

    ImGui::PushItemWidth(100);
    ImGuiHelper::SliderUInt("TAS Speed Mult", &tas->mSpeed, 1, 10);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Breaks some cutscenes if multiple of 5");
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(250);
    ImGuiHelper::DragUInt("Speed Until Frame", &tas->mSpeedupFrame, 1, 0, INT32_MAX);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Stops the speedup at the given frame");
    ImGui::PopItemWidth();
    return true;
}

void WindowTASTools::updateCurrentPattern() {
    for (int i = 0; i < 23; ++i) {
        if (strcmp(mSelectedPattern, patterns[i]) == 0) {
            mCurPattern = i;
            return;
        }
    }
    // If not found, default to Random
    mCurPattern = Random;
}

bool isPatternReverse() {
    WindowTASTools* tools = DevGuiManager::instance()->getWindow<WindowTASTools>("TAS Tools");
    bool b = al::isHalfProbability();
    if (tools && tools->mCurPattern != 0)
        b = tools->mPatternEntries[tools->mCurPattern - 1].reverse;
    return b;
}

int getMofumofuTarget(int a) {
    WindowTASTools* tools = DevGuiManager::instance()->getWindow<WindowTASTools>("TAS Tools");
    int r = al::getRandom(a);
    if (tools && tools->mCurPattern != 0)
        r = tools->mPatternEntries[tools->mCurPattern - 1].target;
    return r;
}

HkTrampoline getPlayerViewMtxHook = [](TrampolineStatic(), PlayerActorHakoniwa* player) -> const sead::Matrix34f& {
    if (STAS::instance() && STAS::instance()->mIsUseAbsJoystick)
        return sead::Matrix34f::ident;
    else
        return *player->getViewMtx();
};

void DevGuiHooks::setupTasHooks() {
    Logger::log("Setting up TAS Tools hooks\n");
    hk::hook::writeBranchLinkAtSym<"$isPatternReverseHook">(isPatternReverse);
    hk::hook::writeBranchLinkAtSym<"$getMofumofuTargetHook">(getMofumofuTarget);
    getPlayerViewMtxHook.installAtSym<"_ZN14PlayerFunction16getPlayerViewMtxEPKN2al9LiveActorE">();
}

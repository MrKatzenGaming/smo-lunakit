#include "devgui/windows/TASTools/WindowTASTools.h"

#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/util/Math.h"

#include "al/Library/Math/MathUtil.h"

#include "custom/game/Player/HackCap.h"

#include <cstdint>

#include "Player/PlayerActorHakoniwa.h"
#include "devgui/DevGuiManager.h"
#include "devgui/windows/WindowBase.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "smo-tas/TAS.h"

WindowTASTools::WindowTASTools(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {}

bool WindowTASTools::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    ImGui::Combo("Wiggler Pattern", &mCurPattern, patterns, hk::util::arraySize(patterns));

    // ImGui::Text("Current Pattern: %s", patterns[mCurPattern]);
    // ImGui::Text("Current Pattern ID: %d", mCurPattern);

    ImGui::Checkbox("Absolute Joystick", TAS::instance()->isUseAbsoluteJoystickPtr());
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Can be toggled using L-Stick while a TAS is running");

    ImGui::PushItemWidth(100);
    ImGui::SliderInt("TAS Speed Mult", TAS::instance()->getSpeedPtr(), 1, 10);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Breaks some cutscenes if multiple of 5");
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(250);
    ImGui::DragInt("Speed Until Frame", TAS::instance()->getSpeedUntilFramePtr(), 1, 0, INT32_MAX);
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
    if (tools->mCurPattern != 0)
        b = tools->mPatternEntries[tools->mCurPattern - 1].reverse;
    return b;
}

int getMofumofuTarget(int a) {
    WindowTASTools* tools = DevGuiManager::instance()->getWindow<WindowTASTools>("TAS Tools");
    int r = al::getRandom(a);
    if (tools->mCurPattern != 0)
        r = tools->mPatternEntries[tools->mCurPattern - 1].target;
    return r;
}

HkTrampoline<const sead::Matrix34f&, PlayerActorHakoniwa*> getPlayerViewMtxHook =
    hk::hook::trampoline([](PlayerActorHakoniwa* player) -> const sead::Matrix34f& {
        if (TAS::instance()->isUseAbsoluteJoystick())
            return sead::Matrix34f::ident;
        else
            return *player->getViewMtx();
    });

void setupTasHooks() {
    Logger::log("Setting up TAS Tools hooks\n");
    hk::hook::writeBranchLinkAtSym<"$isPatternReverseHook">(isPatternReverse);
    hk::hook::writeBranchLinkAtSym<"$getMofumofuTargetHook">(getMofumofuTarget);
    getPlayerViewMtxHook.installAtSym<"_ZN14PlayerFunction16getPlayerViewMtxEPKN2al9LiveActorE">();
}

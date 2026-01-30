#include "devgui/windows/TASTools/WindowTASTools.h"

#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"

#include "al/Library/Math/MathUtil.h"

#include "custom/game/Player/HackCap.h"

#include "Player/PlayerActorHakoniwa.h"
#include "devgui/DevGuiManager.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "smo-tas/TAS.h"

WindowTASTools::WindowTASTools(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {}

void WindowTASTools::updateWin() {}

void WindowTASTools::update() {}

bool WindowTASTools::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    drawDropdown("Wiggler Pattern", patterns, 23, &mSelectedPattern);
    ImGui::Text("Current Pattern: %s", mSelectedPattern);
    ImGui::Text("Current Pattern ID: %d", mCurrPattern);

    return true;
}

void WindowTASTools::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output) {
    if (ImGui::BeginCombo(header, *output, 0)) {
        for (int n = 0; n < totalOptions; n++) {
            bool is_selected = (*output == options[n]);
            if (ImGui::Selectable(options[n], is_selected)) {
                *output = options[n];
                updateCurrentPattern();
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}

void WindowTASTools::updateCurrentPattern() {
    for (int i = 0; i < 23; ++i) {
        if (strcmp(mSelectedPattern, patterns[i]) == 0) {
            mCurrPattern = i;
            return;
        }
    }
    // If not found, default to Random
    mCurrPattern = Random;
}

bool isPatternReverse() {
    WindowTASTools* tools = (WindowTASTools*)DevGuiManager::instance()->getWindow("TAS Tools");
    bool b = al::isHalfProbability();
    if (tools->mCurrPattern != 0)
        b = tools->mPatternEntries[tools->mCurrPattern - 1].reverse == 1;
    return b;
}

int getMofumofuTarget(int a) {
    WindowTASTools* tools = (WindowTASTools*)DevGuiManager::instance()->getWindow("TAS Tools");
    int r = al::getRandom(a);
    if (tools->mCurrPattern != 0)
        r = tools->mPatternEntries[tools->mCurrPattern - 1].target;
    return r;
}

HkTrampoline<const sead::Matrix34f&, PlayerActorHakoniwa*> getPlayerViewMtxHook =
    hk::hook::trampoline([](PlayerActorHakoniwa* player) -> const sead::Matrix34f& {
        if (*TAS::instance()->isUseAbsoluteJoystick())
            return sead::Matrix34f::ident;
        else
            return *player->getViewMtx();
    });

void setupTasHooks() {
    Logger::log("Setting up TAS Tools hooks\n");
    hk::hook::writeBranchLinkAtMainOffset(0x0b07a8, isPatternReverse);
    hk::hook::writeBranchLinkAtMainOffset(0x0b07f8, getMofumofuTarget);
    getPlayerViewMtxHook.installAtSym<"_ZN14PlayerFunction16getPlayerViewMtxEPKN2al9LiveActorE">();
}

#include "program/devgui/windows/TASTools/WindowTASTools.h"
#include "devgui/DevGuiManager.h"

#include "al/util/RandomUtil.h"

#include "imgui.h"
#include "logger/Logger.hpp"

WindowTASTools::WindowTASTools(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault)
{
    setupTasToolsHooks();
}

void WindowTASTools::updateWin()
{
}

bool WindowTASTools::tryUpdateWinDisplay()
{
    if (!WindowBase::tryUpdateWinDisplay())
        return false;
    
    drawDropdown("Wiggler Pattern", patterns, 23, &mSelectedPattern);
    ImGui::Text("Current Pattern: %s", mSelectedPattern);
    ImGui::Text("Current Pattern ID: %d", mCurrPattern);

    return true;
}  

void WindowTASTools::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output)
{
    if(ImGui::BeginCombo(header, *output, 0)) {
        for(int n = 0; n < totalOptions; n++) {
            bool is_selected = (*output == options[n]);
            if (ImGui::Selectable(options[n], is_selected)){ 
                *output = options[n];
                updateCurrentPattern();
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}

bool isPatternReverse()
{   
    WindowTASTools* tools = (WindowTASTools*)DevGuiManager::instance()->getWindow("TAS Tools");
    bool b = al::isHalfProbability();
    if (tools->mCurrPattern != 0)
        b = tools->mPatternEntries[tools->mCurrPattern -1].reverse == 1;
    return b;
}

int getMofumofuTarget(int a)
{
    WindowTASTools* tools = (WindowTASTools*)DevGuiManager::instance()->getWindow("TAS Tools");
    int r = al::getRandom(a);
    if (tools->mCurrPattern != 0)
        r = tools->mPatternEntries[tools->mCurrPattern -1].target;
    return r;
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

void WindowTASTools::setupTasToolsHooks()
{
    Logger::log("Setting up TAS Tools hooks\n");
    exl::patch::CodePatcher(0x0b07a8).BranchLinkInst((void*) isPatternReverse);
    exl::patch::CodePatcher(0x0b07f8).BranchLinkInst((void*) getMofumofuTarget);
}

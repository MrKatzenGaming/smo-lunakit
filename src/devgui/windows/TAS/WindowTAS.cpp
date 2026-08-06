#include "devgui/windows/TAS/WindowTAS.h"

#include "devgui/DevGuiManager.h"
#include "devgui/categories/tas/CategoryTASReplay.h"
#include "devgui/categories/tas/CategoryTASScripts.h"
#include "ghost/GhostManager.h"
#include "helpers/ImGuiHelper.h"
#include "imgui.h"
#include "smo-tas/STAS.h"

WindowTAS::WindowTAS(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {
    createCategory<CategoryTASScripts>("Scripts", "TAS Scripts");
    createCategory<CategoryTASReplay>("Replay", "Replay TASes");
}

bool WindowTAS::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;
    float height = ImGui::GetWindowHeight();
    ImGui::SetCursorPosY(height - ImGui::GetFontSize() * 8.5);
    // TAS* tas = TAS::instance();
    STAS* tas = STAS::instance();
    if (!tas)
        return false;
    auto* ghostManager = GhostManager::instance();
    ImGui::Text("Loaded Script: %s", tas->hasScript() ? tas->getScriptName() : "None.");
    ImGui::Checkbox("TAS", &isStartTAS);
    ImGui::SameLine();
    ImGui::Checkbox("Record", &isStartRecord);
    ImGui::SameLine();
    ImGui::Checkbox("Replay", &isStartReplay);
    if (ImGui::Button("Start")) {
        tas->mIsSpeedupDone = false;
        if (isStartTAS)
            tas->tryStartScript();
        if (isStartRecord)
            ghostManager->tryStartRecord();
        if (isStartReplay)
            ghostManager->tryStartReplay();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Absolute Joystick", &tas->mIsUseAbsJoystick);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Can be toggled using L-Stick while a TAS is running");
    if (ImGui::Button("End All")) {
        tas->endScript();
        ghostManager->endRecord();
        ghostManager->endReplay();
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGuiHelper::SliderUInt("Speed Mult", &tas->mSpeed, 1, 10);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Breaks some cutscenes if multiple of 5");
    ImGui::PopItemWidth();
    if (ImGui::Button("Refresh Directories")) {
        tas->updateDir();
        ghostManager->updateDir();
    }

    if (tas->isRunning())
        ImGui::Text("TAS is running (%ld/%u)", tas->getCurFrame(), tas->getFrameCount());
    else
        ImGui::Text("TAS is not running.");

    return true;
}

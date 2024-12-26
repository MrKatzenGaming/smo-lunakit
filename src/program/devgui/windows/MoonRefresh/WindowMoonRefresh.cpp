#include "program/devgui/windows/MoonRefresh/WindowMoonRefresh.h"
#include "devgui/DevGuiManager.h"

#include "devgui/savedata/DevGuiSaveData.h"
#include "helpers/GetHelper.h"

#include "imgui.h"

WindowMoonRefresh::WindowMoonRefresh(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault)
{
}
bool WindowMoonRefresh::mIsGrayRefreshEnabled = false;
bool WindowMoonRefresh::mIsRefreshEnabled = false;
char* WindowMoonRefresh::mRefreshText = "///////////////////////";

void WindowMoonRefresh::updateWin()
{
}

bool WindowMoonRefresh::tryUpdateWinDisplay()
{
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    if(ImGui::Checkbox("Reactivate Moons", &mIsGrayRefreshEnabled)){
        mParent->getSaveData()->queueSaveWrite();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("If enabled, moons will be reactivated after being collected.\nIf disabled, moons will remain grayed out after being collected.");
    ImGui::SameLine();
    if(ImGui::Checkbox("Disable Saving Moons", &mIsRefreshEnabled)){
        mParent->getSaveData()->queueSaveWrite();
    }
    if(ImGui::IsItemHovered())
        ImGui::SetTooltip("If enabled, moons will not be saved to the save file.\nIf disabled, moons will be saved to the save file.");

    if (mIsRefreshEnabled) {
        
       if ( ImGui::InputText("Refresh Text", mRefreshText, 51)) {
            mParent->getSaveData()->queueSaveWrite();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("The text that will be displayed when a moon is collected.");
        if(ImGui::Button("Set Text")){
            mParent->tryOpenKeyboard(50, KEYTYPE_QWERTY, (const char**)&mRefreshText, &mIsKeyboardInUse);
            mWasKeyboardOpen = mIsKeyboardInUse;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Set the text that will be displayed when a moon is collected.");
        ImGui::SameLine();
        if(ImGui::Button("Reset Text")){
            strncpy(mRefreshText, "///////////////////////", 24);
            mParent->getSaveData()->queueSaveWrite();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Reset the text that will be displayed when a moon is collected.");
    }
    if (mWasKeyboardOpen && !mIsKeyboardInUse) {
    mParent->getSaveData()->queueSaveWrite();
    mWasKeyboardOpen = false;
}
return true;
}





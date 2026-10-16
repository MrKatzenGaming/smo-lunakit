#include "devgui/windows/WindowTASInfo.h"

#include "imgui.h"
#include "smo-tas/STAS.h"
#include "smo-tas/Script.h"

WindowTASInfo::WindowTASInfo(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {}

bool WindowTASInfo::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    STAS* tas = STAS::instance();
    if (!tas)
        return false;

    Script* script = tas->getScript();
    if (!script)
        return false;

    ImGui::Text("Title: %s", script->getTitle());
    ImGui::TextWrapped("Author(s): %s", script->getAuthor());
    ImGui::TextWrapped("Desciption: %s", script->getDesc());
    ImGui::Text("Current Mario TASer: %s", tas->getCurMarTASer());
    ImGui::Text("Current Cappy TASer: %s", tas->getCurCapTASer());

    return true;
}

void WindowTASInfo::updateWin() {
    WindowBase::updateWin();
}

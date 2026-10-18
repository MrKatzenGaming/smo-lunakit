#include "devgui/categories/tas/CategoryTASScripts.h"

#include "sead/prim/seadSafeString.h"

#include "imgui.h"
#include "smo-tas/STAS.h"

CategoryTASScripts::CategoryTASScripts(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryTASScripts::updateCatDisplay() {
    CategoryBase::updateCatDisplay();
    auto* tas = STAS::instance();
    if (!tas)
        return;

    ImGui::BeginChild("Scripts", ImVec2(0, -(ImGui::GetFontSize() * 8.5)), true);
    s64 entryCount = tas->getEntryCount();
    nn::fs::DirectoryEntry* entries = tas->getScripts();
    for (int i = 0; i < entryCount; i++) {
        sead::FormatFixedSafeString<0x20> a("TASScript%d", i);
        ImGui::PushID(a.cstr());
        bool isClicked = ImGui::Button(entries[i].mName);
        ImGui::PopID();

        if (isClicked) {
            // Logger::log("2 Script Name: %s\n", tas->getScriptName());
            tas->setScript(entries[i]);
            // Logger::log("3 Script Name: %s\n", tas->getScriptName());
            tas->tryLoadScript();
            break;
        }
    }
    ImGui::EndChild();
}

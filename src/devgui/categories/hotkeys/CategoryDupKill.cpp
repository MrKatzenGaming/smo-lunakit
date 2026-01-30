#include "devgui/categories/hotkeys/CategoryDupKill.h"

#include "Library/Base/StringUtil.h"

#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "helpers/PlayerHelper.h"
#include "imgui.h"

CategoryDupKill::CategoryDupKill(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryDupKill::updateCat() {
    if (mIsOverride) {
        // check if keys are pressed
        if (al::isEqualString(mKey1, "None"))
            isKey1Pressed = true;
        if (al::isEqualString(mKey1, "ZL"))
            isKey1Pressed = InputHelper::isHoldZL();
        if (al::isEqualString(mKey1, "ZR"))
            isKey1Pressed = InputHelper::isHoldZR();
        if (al::isEqualString(mKey1, "L"))
            isKey1Pressed = InputHelper::isHoldL();
        if (al::isEqualString(mKey1, "R"))
            isKey1Pressed = InputHelper::isHoldR();
        if (al::isEqualString(mKey2, "None"))
            isKey2Pressed = true;
        if (al::isEqualString(mKey2, "ZL"))
            isKey2Pressed = InputHelper::isHoldZL();
        if (al::isEqualString(mKey2, "ZR"))
            isKey2Pressed = InputHelper::isHoldZR();
        if (al::isEqualString(mKey2, "L"))
            isKey2Pressed = InputHelper::isHoldL();
        if (al::isEqualString(mKey2, "R"))
            isKey2Pressed = InputHelper::isHoldR();

        // check if only enabled keys are pressed
        if ((InputHelper::isHoldZL() && !al::isEqualString(mKey1, "ZL") && !al::isEqualString(mKey2, "ZL")) ||
            (InputHelper::isHoldZR() && !al::isEqualString(mKey1, "ZR") && !al::isEqualString(mKey2, "ZR")) ||
            (InputHelper::isHoldL() && !al::isEqualString(mKey1, "L") && !al::isEqualString(mKey2, "L")) ||
            (InputHelper::isHoldR() && !al::isEqualString(mKey1, "R") && !al::isEqualString(mKey2, "R"))) {
            return;
        }

        // get needed stuff
        HakoniwaSequence* gameSeq = tryGetHakoniwaSequence();
        PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa(gameSeq);

        // check if player exists
        if (!player)
            return;
        // kill player if enabled keys and Dpad Up are pressed
        if (isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp())
            PlayerHelper::killPlayer(player);
    }
}

void CategoryDupKill::updateCatDisplay() {
    CategoryBase::updateCatDisplay();

    // draw the checkbox to enable/disable the category
    if (ImGui::Checkbox("Enable", &mIsOverride)) {
    }

    if (!mIsOverride)
        return;
    // draw the dropdowns for the keys
    drawDropdown("Key 1", KeyNames, mTotalKeys, &mKey1);
    drawDropdown("Key 2", KeyNames, mTotalKeys, &mKey2);
}

void CategoryDupKill::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output) {
    if (ImGui::BeginCombo(header, *output, mComboFlags)) {
        for (int n = 0; n < mTotalKeys; n++) {
            bool is_selected = (*output == options[n]);
            if (ImGui::Selectable(options[n], is_selected))
                *output = options[n];
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}

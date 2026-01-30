#include "devgui/categories/hotkeys/CategorySetHP.h"

#include "al/Library/Base/StringUtil.h"

#include "game/System/GameDataFile.h"

#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "imgui.h"

CategorySetHP::CategorySetHP(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategorySetHP::updateCat() {
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

        // get needed stuff
        HakoniwaSequence* gameSeq = tryGetHakoniwaSequence();
        PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa(gameSeq);
        mHitData = gameSeq->mGameDataHolderAccessor.mData->getGameDataFile()->mPlayerHitPointData;

        // check if player exists
        if (!player)
            return;
        // kill player if enabled keys and Dpad Up are pressed
        if (isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp() && !mIsGetLifeUp)
            mHitData->mCurrentHealth = mTargetHealth;
        else if (isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp() && mIsGetLifeUp)
            mHitData->getMaxUpItem();
        if (mIsOverride)
            mHitData->mIsKidsMode = mIsKidsMode;
    }
}

void CategorySetHP::updateCatDisplay() {
    CategoryBase::updateCatDisplay();

    // draw the checkbox to enable/disable the category
    if (ImGui::Checkbox("Enable", &mIsOverride)) {
    }

    if (!mIsOverride)
        return;
    // draw the dropdowns for the keys
    drawDropdown("Key 1", KeyNames, mTotalKeys, &mKey1);
    drawDropdown("Key 2", KeyNames, mTotalKeys, &mKey2);

    ImGui::Checkbox("Kids", &mIsKidsMode);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Kids Mode is Assist Mode\nOnly overrides health meter,\nnot all of Assist Mode!");

    ImGui::SameLine();
    ImGui::Checkbox("Extend", &mIsExtendSlider);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Allows entering higher HP\nvalues than what is possible\nin the base game");

    ImGui::SameLine();
    ImGui::Checkbox("Life Up", &mIsGetLifeUp);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Gives you a Life Up\ninstead of setting your HP\nvalue to the one you entered");

    ImGui::SliderInt("Health", &mTargetHealth, 1, mIsExtendSlider ? 255 : 9);
}

void CategorySetHP::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output) {
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

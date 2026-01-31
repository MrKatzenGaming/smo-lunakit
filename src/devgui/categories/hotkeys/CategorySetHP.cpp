#include "devgui/categories/hotkeys/CategorySetHP.h"

#include "game/System/GameDataFile.h"

#include "devgui/categories/hotkeys/CategoryHotkeyBase.h"
#include "helpers/GetHelper.h"
#include "imgui.h"

CategorySetHP::CategorySetHP(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryHotkeyBase(catName, catDesc, heap) {}

void CategorySetHP::hotkeyFunc() {
    HakoniwaSequence* gameSeq = tryGetHakoniwaSequence();
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa(gameSeq);
    mHitData = gameSeq->mGameDataHolderAccessor.mData->getGameDataFile()->mPlayerHitPointData;

    // check if player exists
    if (!player || !gameSeq || !mHitData)
        return;
    // kill player if enabled keys and Dpad Up are pressed
    if (!mIsGetLifeUp)
        mHitData->mCurrentHealth = mTargetHealth;
    else if (mIsGetLifeUp)
        mHitData->getMaxUpItem();
    if (mIsOverride)
        mHitData->mIsKidsMode = mIsKidsMode;
}

void CategorySetHP::updateCatDisplay() {
    CategoryHotkeyBase::updateCatDisplay();

    if (!mIsOverride)
        return;

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

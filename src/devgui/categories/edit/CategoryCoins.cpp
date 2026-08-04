#include "devgui/categories/edit/CategoryCoins.h"

#include "hk/ro/RoUtil.h"

#include "game/Layout/CoinCounter.h"
#include "game/Layout/StageSceneLayout.h"
#include "game/System/GameDataFile.h"

#include "helpers/GetHelper.h"
#include "imgui.h"

CategoryCoins::CategoryCoins(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryCoins::updateCat() {
    if (mIsOverrideCoins) {
        GameDataHolder* holder = tryGetGameDataHolder();

        if (!holder)
            return;
        holder->getGameDataFile()->mCoinNum = mTargetCoins;

        StageScene* scene = tryGetStageScene();
        if (!scene)
            return;

        if (hk::ro::getMainModule()->isVersion("120"))
            (*(&scene->mStageSceneLayout + 2))->mCoinCounter->updateCountImmidiate();
        else
            scene->mStageSceneLayout->mCoinCounter->updateCountImmidiate();
    }
}

void CategoryCoins::updateCatDisplay() {
    CategoryBase::updateCatDisplay();

    GameDataHolder* holder = tryGetGameDataHolder();
    if (!holder)
        return;

    if (ImGui::Checkbox("Edit Coins", &mIsOverrideCoins))
        mTargetCoins = holder->getGameDataFile()->mCoinNum;

    if (mIsOverrideCoins) {
        ImGui::SameLine();
        ImGui::Checkbox("Negative", &mIsExtendSlider);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Allows entering negative Coin\nvalues which is not possible\nin the base game");

        ImGui::SliderInt("Coins", &mTargetCoins, mIsExtendSlider ? -9999 : 0, 9999);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("The amount of Coins the player has");
    }
}

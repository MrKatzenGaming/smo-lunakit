#include "devgui/categories/edit/CategoryJump.h"

#include "hk/ro/RoUtil.h"

#include "game/Player/PlayerConst.h"

#include "helpers/GetHelper.h"
#include "imgui.h"

CategoryJump::CategoryJump(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryJump::updateCat() {
    // Get the player actor and check if they are dead
    PlayerActorHakoniwa* playerHak = tryGetPlayerActorHakoniwa();
    if (!playerHak)
        return;

    // Reset the player's speed consts, before overriding them with new value if in use
    if (hk::ro::getMainModule()->isVersion("120")) {
        *(&playerHak->mConst->mJumpPowerMin + 1) = 17;
        *(&playerHak->mConst->mJumpPowerMax + 1) = 19.5;
    } else {
        playerHak->mConst->mJumpPowerMin = 17;
        playerHak->mConst->mJumpPowerMax = 19.5;
    }

    if (!mIsOverride)
        return;

    if (hk::ro::getMainModule()->isVersion("120")) {
        *(&playerHak->mConst->mJumpPowerMin + 1) = (19.5f / 17.0f) * mTargetJump;
        *(&playerHak->mConst->mJumpPowerMax + 1) = mTargetJump;
    } else {
        playerHak->mConst->mJumpPowerMin = (19.5f / 17.0f) * mTargetJump;
        playerHak->mConst->mJumpPowerMax = mTargetJump;
    }
}

void CategoryJump::updateCatDisplay() {
    CategoryBase::updateCatDisplay();

    ImGui::Checkbox("Edit Jump", &mIsOverride);

    if (mIsOverride)
        ImGui::DragFloat("Jump", &mTargetJump, 1.f, -FLT_MAX, FLT_MAX, "%.3f");

    if (mIsOverride && ImGui::Button("Reset Jump"))
        mTargetJump = 17.0f;
}

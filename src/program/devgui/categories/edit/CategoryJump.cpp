#include "program/devgui/categories/edit/CategoryJump.h"

#include "helpers/GetHelper.h"

#include "logger/Logger.hpp"

#include "imgui.h"

CategoryJump::CategoryJump(const char* catName, const char* catDesc, sead::Heap* heap)
    : CategoryBase(catName, catDesc, heap)
{}

void CategoryJump::updateCat()
{
    // Get the player actor and check if they are dead
    PlayerActorHakoniwa* playerHak = tryGetPlayerActorHakoniwa();
    if (!playerHak)
        return;

    // Reset the player's speed consts, before overriding them with new value if in use
    playerHak->mPlayerConst->mJumpPowerMin = 17;
    playerHak->mPlayerConst->mJumpPowerMax = 19.5;


    if (!mIsOverride)
        return;

    playerHak->mPlayerConst->mJumpPowerMin = mTargetJump;
    playerHak->mPlayerConst->mJumpPowerMax = mTargetJump;
}

void CategoryJump::updateCatDisplay()
{
    CategoryBase::updateCatDisplay();

    ImGui::Checkbox("Edit Jump", &mIsOverride);

    if (mIsOverride)
        ImGui::DragFloat("Jump", &mTargetJump, 1.f, -FLT_MAX, FLT_MAX, "%.3f");

    if (mIsOverride && ImGui::Button("Reset Jump"))
        mTargetJump = 17.0f;
}
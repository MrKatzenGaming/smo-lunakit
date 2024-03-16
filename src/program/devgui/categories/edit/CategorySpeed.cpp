#include "program/devgui/categories/edit/CategorySpeed.h"

#include "game/Player/PlayerConst.h"

#include "helpers/GetHelper.h"

#include "logger/Logger.hpp"

#include "imgui.h"

CategorySpeed::CategorySpeed(const char* catName, const char* catDesc, sead::Heap* heap)
    : CategoryBase(catName, catDesc, heap)
{}

void CategorySpeed::updateCat()
{
    // Get the player actor and check if they are dead
    PlayerActorHakoniwa* playerHak = tryGetPlayerActorHakoniwa();
    if (!playerHak)
        return;

    // Reset the player's speed consts, before overriding them with new value if in use
    playerHak->mConst->mNormalMaxSpeed = 14;
    playerHak->mConst->mJumpBaseSpeedMax = 24;
    playerHak->mConst->mJumpMoveSpeedMax = 30;
    playerHak->mConst->mSquatWalkSpeed = 3.5;

    if (!mIsOverride)
        return;

    playerHak->mConst->mNormalMaxSpeed = mTargetSpeed;
    playerHak->mConst->mJumpBaseSpeedMax = mTargetSpeed * (24.0f / 14.0f);
    playerHak->mConst->mJumpMoveSpeedMax = mTargetSpeed * (30.0f / 14.0f);
    playerHak->mConst->mSquatWalkSpeed = mTargetSpeed * (3.5f / 14.0f);
}

void CategorySpeed::updateCatDisplay()
{
    CategoryBase::updateCatDisplay();

    ImGui::Checkbox("Edit Speed", &mIsOverride);

    if (mIsOverride)
        ImGui::DragFloat("Speed", &mTargetSpeed, 1.f, -FLT_MAX, FLT_MAX, "%.3f");

    if (mIsOverride && ImGui::Button("Reset Speed"))
        mTargetSpeed = 14.0f;
}

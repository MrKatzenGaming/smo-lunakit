#include "program/devgui/categories/info/CategoryInfStats.h"
#include "rs/util.hpp"
#include "helpers/GetHelper.h"
#include "game/System/GameDataFunction.h"

#include "imgui.h"

CategoryInfStats::CategoryInfStats(const char* catName, const char* catDesc, sead::Heap* heap)
    : CategoryBase(catName, catDesc, heap) {}

void CategoryInfStats::updateCatDisplay()
{
    StageScene* stageScene = tryGetStageScene();
    // GameDataHolderAccessor* accessor = tryGetGameDataHolderAccess();

    if(!stageScene) {
        ImGui::Text("StageScene does not exist!");
        return;
    }

    jumpCount = rs::getPlayerJumpCount(stageScene->mGameDataHolder.mData);
    throwCapCount = rs::getPlayerThrowCapCount(stageScene->mGameDataHolder.mData);
    playTimeTotal = GameDataFunction::getPlayTimeTotal(stageScene->mGameDataHolder);
    playTimeAcrossFile = GameDataFunction::getPlayTimeAcrossFile(stageScene->mGameDataHolder);
    totalCoinNum = rs::getTotalCoinNum(stageScene->mGameDataHolder.mData);

    ImGui::Text("Jump Count: %d", jumpCount);
    ImGui::Text("Throw Cap Count: %d", throwCapCount);
    ImGui::Text("Total Play Time: %d", playTimeTotal);
    ImGui::Text("Play Time Across File: %d", playTimeAcrossFile);
    ImGui::Text("Total Coin Num: %d", totalCoinNum);

    return;
}

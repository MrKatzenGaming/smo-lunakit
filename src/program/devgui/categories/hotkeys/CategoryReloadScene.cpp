#include "program/devgui/categories/hotkeys/CategoryReloadScene.h"

#include "Library/LiveActor/ActorPoseKeeper.h"
#include "al/util.hpp"
#include "al/util/LiveActorUtil.h"
#include "helpers/GetHelper.h"
#include "helpers/PlayerHelper.h"
#include "helpers/InputHelper.h"
#include "game/Player/PlayerFunction.h"
#include "logger/Logger.hpp"
#include "game/System/GameDataFunction.h"
#include "game/System/GameDataFile.h"

#include "imgui.h"

CategoryReloadScene::CategoryReloadScene(const char* catName, const char* catDesc, sead::Heap* heap)
    : CategoryBase(catName, catDesc, heap)
{
}

void CategoryReloadScene::updateCat()
{
    if(mIsOverride) {

        //check if keys are pressed
        if(mKey1 == "None") isKey1Pressed = true; if(mKey1 == "ZL") isKey1Pressed = InputHelper::isHoldZL(); if(mKey1 == "ZR") isKey1Pressed = InputHelper::isHoldZR(); if(mKey1 == "L") isKey1Pressed = InputHelper::isHoldL(); if(mKey1 == "R") isKey1Pressed = InputHelper::isHoldR();
        if(mKey2 == "None") isKey2Pressed = true; if(mKey2 == "ZL") isKey2Pressed = InputHelper::isHoldZL(); if(mKey2 == "ZR") isKey2Pressed = InputHelper::isHoldZR(); if(mKey2 == "L") isKey2Pressed = InputHelper::isHoldL(); if(mKey2 == "R") isKey2Pressed = InputHelper::isHoldR();
    
    //check if only enabled keys are pressed
    if ((InputHelper::isHoldZL() && mKey1 != "ZL" && mKey2 != "ZL") || 
    (InputHelper::isHoldZR() && mKey1 != "ZR" && mKey2 != "ZR") || 
    (InputHelper::isHoldL() && mKey1 != "L" && mKey2 != "L") || 
    (InputHelper::isHoldZR() && mKey1 != "R" && mKey2 != "R")) {
    return;
    }
    //get needed stuff             
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();
    StageScene* stageScene = tryGetStageScene();
    GameDataHolder* holder = tryGetGameDataHolder();
    GameDataHolderAccessor* accessor = tryGetGameDataHolderAccess();

    //check if player exists
    if (!player)
        return;
        
        //stage reload code if position reloading is enabled (has to run in a constant loop)
        if (reloadStageForPos != -1) {
        reloadStageForPos++;
            if (reloadStageForPos == 11) {
                player->startDemoPuppetable();
                al::setTrans(player, reloadStageTrans);
                al::updatePoseQuat(player, reloadStageQuat);
                al::setVelocityZero(player);
                player->endDemoPuppetable();
                reloadStageForPos = -1;
            }
        }

        //reload stage at entrance if position reloading is disabled
        if (isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp() && !mIsLoadPos){
        StageScene* stageScene = tryGetStageScene();
        stageScene->kill();
        }

        //reload stage at current position if position reloading is enabled
        if(isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp() && mIsLoadPos){
                reloadStageForPos = 0;
                    if (player) {
                        reloadStageTrans = al::getTrans(player);
                        reloadStageQuat = al::getQuat(player);
                    }
            const char* entry = accessor->mData->mGameDataFile->mPlayerStartId.cstr();
            ChangeStageInfo info = ChangeStageInfo(holder, "start" , GameDataFunction::getCurrentStageName(stageScene->mGameDataHolder), false, -1, ChangeStageInfo::SubScenarioType::UNK);
            holder->changeNextStage(&info, 0);
        }
}

}

void CategoryReloadScene::updateCatDisplay()
{
    CategoryBase::updateCatDisplay();

//draw checkbox to enable/disable the hotkey
    ImGui::Checkbox("Enable", &mIsOverride);
    if(ImGui::IsItemHovered())
        ImGui::SetTooltip("Enables the hotkey\nto reload the scene");
    

    if (!mIsOverride) 
        return;
        
        //draw checkbox to enable/disable position reloading
        ImGui::Checkbox("Reload At Position", &mIsLoadPos);
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Reloads the scene at the position\nyou are currently at\n(will \"break\" reload if disabled again)");

        //draw dropdowns to select the keys
        drawDropdown("Key 1", KeyNames, mTotalKeys, &mKey1);
        drawDropdown("Key 2", KeyNames, mTotalKeys, &mKey2);

}

void CategoryReloadScene::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output)
{
    if(ImGui::BeginCombo(header, *output, mComboFlags)) {
        for(int n = 0; n < mTotalKeys; n++) {
            bool is_selected = (*output == options[n]); 
            if (ImGui::Selectable(options[n], is_selected))
                *output = options[n];
            if (is_selected)
                ImGui::SetItemDefaultFocus();   
        }

        ImGui::EndCombo();
    }
}
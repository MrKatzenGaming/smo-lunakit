#include "program/devgui/categories/hotkeys/CategoryDupKill.h"

#include "helpers/GetHelper.h"
#include "helpers/PlayerHelper.h"
#include "game/Player/PlayerFunction.h"
#include "logger/Logger.hpp"
#include "game/GameData/GameDataFunction.h"

#include "imgui.h"

CategoryDupKill::CategoryDupKill(const char* catName, const char* catDesc, sead::Heap* heap)
    : CategoryBase(catName, catDesc, heap)
{
}

void CategoryDupKill::updateCat()
{

    
    if(mIsEnabled) {

 //check if keys are pressed
        if(mKey1 == "None") isKey1Pressed = true; if(mKey1 == "ZL") isKey1Pressed = al::isPadHoldZL(-1); if(mKey1 == "ZR") isKey1Pressed = al::isPadHoldZR(-1); if(mKey1 == "L") isKey1Pressed = al::isPadHoldL(-1); if(mKey1 == "R") isKey1Pressed = al::isPadHoldR(-1);
        if(mKey2 == "None") isKey2Pressed = true; if(mKey2 == "ZL") isKey2Pressed = al::isPadHoldZL(-1); if(mKey2 == "ZR") isKey2Pressed = al::isPadHoldZR(-1); if(mKey2 == "L") isKey2Pressed = al::isPadHoldL(-1); if(mKey2 == "R") isKey2Pressed = al::isPadHoldR(-1);
    
    //check if only enabled keys are pressed
    if ((al::isPadHoldZL(-1) && mKey1 != "ZL" && mKey2 != "ZL") || 
    (al::isPadHoldZR(-1) && mKey1 != "ZR" && mKey2 != "ZR") || 
    (al::isPadHoldL(-1) && mKey1 != "L" && mKey2 != "L") || 
    (al::isPadHoldR(-1) && mKey1 != "R" && mKey2 != "R")) {
    return;
    }

    //get needed stuff
    HakoniwaSequence* gameSeq = tryGetHakoniwaSequence();
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa(gameSeq);

    //check if player exists
    if (!player)
        return;
        //kill player if enabled keys and Dpad Up are pressed
        if (isKey1Pressed && isKey2Pressed && al::isPadTriggerUp(-1)){
            PlayerHelper::killPlayer(player);
        }
}

}
void CategoryDupKill::updateCatDisplay()
{
    CategoryBase::updateCatDisplay();

//draw the checkbox to enable/disable the category
    if(ImGui::Checkbox("Enable", &mIsEnabled)){
    }

    if (!mIsEnabled) 
        return;
        //draw the dropdowns for the keys
        drawDropdown("Key 1", KeyNames, mTotalKeys, &mKey1);
        drawDropdown("Key 2", KeyNames, mTotalKeys, &mKey2);

}

void CategoryDupKill::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output)
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
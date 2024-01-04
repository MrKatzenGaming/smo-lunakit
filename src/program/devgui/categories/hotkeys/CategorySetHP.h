    #pragma once

#include "program/devgui/categories/CategoryBase.h"

#include "game/GameData/PlayerHitPointData.h"

typedef int ImGuiComboFlags; // Forward declaration

class CategorySetHP : public CategoryBase {
public:
    CategorySetHP(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCat() override;
    void updateCatDisplay() override;

private:
    void drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output);

    int mTotalKeys = 5;
    const char* KeyNames[5] = {"None","ZL", "ZR", "L", "R"};

    ImGuiComboFlags mComboFlags = 0;

    const char* mKey1 = "None";
    const char* mKey2 = "None";

    bool mIsOverride = false;

    PlayerHitPointData* mHitData;
    bool mIsKidsMode = false;
    bool mIsExtendSlider = false;
    int mTargetHealth = 3;
    bool mIsGetLifeUp = false;

    bool isKey1Pressed = false;
    bool isKey2Pressed = false;
};
    
    
    
    
    
    
    
    
    
    
    

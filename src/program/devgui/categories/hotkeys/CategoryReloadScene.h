#pragma once

#include "program/devgui/categories/CategoryBase.h"

#include "sead/math/seadQuat.h"
#include "sead/math/seadVector.h"

typedef int ImGuiComboFlags; // Forward declaration

class CategoryReloadScene : public CategoryBase {
public:
    CategoryReloadScene(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCat() override;
    void updateCatDisplay() override;

private:
    void drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output);

    int mTotalKeys = 5;
    const char* KeyNames[5] = {"None","ZL", "ZR", "L", "R"};

    ImGuiComboFlags mComboFlags = 0;

    const char* mKey1 = "None";
    const char* mKey2 = "None";

    bool mIsEnabled = false;
    bool mIsLoadPos = false;

    sead::Vector3f reloadStageTrans = sead::Vector3f::zero;
    sead::Quatf reloadStageQuat = sead::Quatf::unit;
    int reloadStageForPos = -1;

    bool isKey1Pressed = false;
    bool isKey2Pressed = false;
};
    
    
    
    
    
    
    
    
    
    
    

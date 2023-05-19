    #pragma once

#include "program/devgui/categories/CategoryBase.h"

typedef int ImGuiComboFlags; // Forward declaration

class CategoryDupKill : public CategoryBase {
public:
    CategoryDupKill(const char* catName, const char* catDesc, sead::Heap* heap);

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

    bool isKey1Pressed = false;
    bool isKey2Pressed = false;
};
    
    
    
    
    
    
    
    
    
    
    

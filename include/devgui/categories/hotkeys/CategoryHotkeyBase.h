#pragma once

#include "devgui/categories/CategoryBase.h"

class CategoryHotkeyBase : public CategoryBase {
public:
    CategoryHotkeyBase(const char* catName, const char* catDesc, sead::Heap* heap);

    virtual void updateCat() override;
    virtual void updateCatDisplay() override;
    virtual void hotkeyFunc();

    bool mIsOverride = false;

private:
    const char* KeyNames[5] = {"None", "ZL", "ZR", "L", "R"};

    enum keyName : int { None, ZL, ZR, L, R };

    const keyName mKey1 = None;
    const keyName mKey2 = None;

    bool isKey1Pressed = false;
    bool isKey2Pressed = false;
};

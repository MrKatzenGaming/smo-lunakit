#pragma once

#include "program/devgui/categories/CategoryBase.h"

class CategoryJump : public CategoryBase {
public:
    CategoryJump(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCat() override;
    void updateCatDisplay() override;

private:
    bool mIsOverride = false;
    float mTargetJump = 17.0f;
};
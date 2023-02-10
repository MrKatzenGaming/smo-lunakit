#pragma once

#include "imgui.h"
#include "program/SequenceUtil.h"

class CategoryBase {
public:
    CategoryBase(const char* catName, const char* catDesc);

    virtual void updateCat();
    virtual void updateCatDisplay();

    virtual const char* getCategoryName() { return mCatName; };

protected:
    const char* mCatName = nullptr;
    const char* mCatDesc = nullptr;
};
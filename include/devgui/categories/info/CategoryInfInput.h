#pragma once

#include "devgui/categories/CategoryBase.h"

class CategoryInfInput : public CategoryBase {
public:
    CategoryInfInput(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;

private:
    char textBuffer[64];
};

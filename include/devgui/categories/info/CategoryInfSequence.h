#pragma once

#include "devgui/categories/CategoryBase.h"

class CategoryInfSequence : public CategoryBase {
public:
    CategoryInfSequence(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;
};

#pragma once

#include "devgui/categories/CategoryBase.h"

class CategoryInfSensors : public CategoryBase {
public:
    CategoryInfSensors(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;

private:
    int sliderValue = 2;
    char format[16];
    char textBuffer[64];
};

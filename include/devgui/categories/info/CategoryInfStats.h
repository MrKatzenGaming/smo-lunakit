#pragma once

#include "al/Library/Scene/Scene.h"

#include "game/System/GameDataHolderAccessor.h"

#include "devgui/categories/CategoryBase.h"

class CategoryInfStats : public CategoryBase {
public:
    CategoryInfStats(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;

private:
    int jumpCount = 0;
    int throwCapCount = 0;
    int playTimeTotal = 0;
    int playTimeAcrossFile = 0;
    int totalCoinNum = 0;
};

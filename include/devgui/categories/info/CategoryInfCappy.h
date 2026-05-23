#pragma once

#include "sead/math/seadQuat.h"
#include "sead/math/seadVector.h"

#include "Library/LiveActor/LiveActorFlag.h"

#include "game/Player/PlayerActorHakoniwa.h"

#include "devgui/categories/CategoryBase.h"

class CategoryInfCappy : public CategoryBase {
public:
    CategoryInfCappy(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;

private:
    void drawActorInspectorTreePose(PlayerActorHakoniwa* player);
    sead::Vector3f QuatToEuler(sead::Quatf* quat);
    int sliderValue = 2;
    char format[16] = {"\0"};
    bool mUseDeg = true;
};

#pragma once

#include "program/devgui/categories/CategoryBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "Library/LiveActor/LiveActorFlag.h"
#include "sead/math/seadQuat.h"
#include "sead/math/seadVector.h"

class CategoryInfPlayer : public CategoryBase {
public:
    CategoryInfPlayer(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;

private:
    void drawActorInspectorTreePose(PlayerActorHakoniwa* player);
    sead::Vector3f QuatToEuler(sead::Quatf *quat);
    int sliderValue = 2;
    char format[16];
};
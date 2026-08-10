#pragma once

#include "sead/math/seadVector.h"

#include "devgui/categories/CategoryBase.h"

class CategoryInfPlayer : public CategoryBase {
public:
    CategoryInfPlayer(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;
    void updateCat() override;

private:
    int sliderValue = 2;
    char format[16];
    char textBuffer[64];
    bool mUseDeg = true;
    sead::Vector3f mRelativeTrans = sead::Vector3f::zero;
    float mRelativeAngle = 0;
    bool mFixedOrigin = false;

    void setRelativeAxes(sead::Vector3f trans, float angle) {
        mRelativeTrans = trans;
        mRelativeAngle = angle;
    }

    sead::Vector3f transformToRelativeAxes(sead::Vector3f vec, bool isTransVector);
    sead::Vector3f transformEulerAnglesToRelativeAxes(sead::Vector3f vec);
};

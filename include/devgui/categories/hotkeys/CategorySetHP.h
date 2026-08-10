#pragma once

#include "game/System/PlayerHitPointData.h"

#include "devgui/categories/hotkeys/CategoryHotkeyBase.h"

class CategorySetHP : public CategoryHotkeyBase {
public:
    CategorySetHP(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCatDisplay() override;
    void hotkeyFunc() override;

private:
    PlayerHitPointData* mHitData;
    bool mIsKidsMode = false;
    bool mIsExtendSlider = false;
    int mTargetHealth = 3;
    bool mIsGetLifeUp = false;
};

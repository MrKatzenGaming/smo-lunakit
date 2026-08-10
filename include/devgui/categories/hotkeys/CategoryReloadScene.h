#pragma once

#include "sead/math/seadQuat.h"

#include "devgui/categories/hotkeys/CategoryHotkeyBase.h"

class CategoryReloadScene : public CategoryHotkeyBase {
public:
    CategoryReloadScene(const char* catName, const char* catDesc, sead::Heap* heap);
    void updateCat() override;
    void updateCatDisplay() override;

    void hotkeyFunc() override;

private:
    bool mIsLoadPos = false;

    sead::Vector3f reloadStageTrans = sead::Vector3f::zero;
    sead::Quatf reloadStageQuat = sead::Quatf::unit;
    int reloadStageForPos = -1;
};

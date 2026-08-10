#include "devgui/categories/hotkeys/CategoryHotkeyBase.h"

#include "hk/util/Algorithm.h"

#include "helpers/InputHelper.h"
#include "imgui.h"

CategoryHotkeyBase::CategoryHotkeyBase(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryHotkeyBase::updateCat() {
    if (!mIsOverride)
        return;

    isKey1Pressed = (mKey1 == None) || (mKey1 == ZL && InputHelper::isHoldZL()) || (mKey1 == ZR && InputHelper::isHoldZR()) ||
                    (mKey1 == L && InputHelper::isHoldL()) || (mKey1 == R && InputHelper::isHoldR());

    isKey2Pressed = (mKey2 == None) || (mKey2 == ZL && InputHelper::isHoldZL()) || (mKey2 == ZR && InputHelper::isHoldZR()) ||
                    (mKey2 == L && InputHelper::isHoldL()) || (mKey2 == R && InputHelper::isHoldR());

    // check if only enabled keys are pressed
    if ((InputHelper::isHoldZL() && mKey1 != ZL && mKey2 != ZL) || (InputHelper::isHoldZR() && mKey1 != ZR && mKey2 != ZR) ||
        (InputHelper::isHoldL() && mKey1 != L && mKey2 != L) || (InputHelper::isHoldR() && mKey1 != R && mKey2 != R)) {
        return;
    }

    if (isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp())
        hotkeyFunc();
}

void CategoryHotkeyBase::updateCatDisplay() {
    CategoryBase::updateCatDisplay();

    // draw the checkbox to enable/disable the category
    if (ImGui::Checkbox("Enable", &mIsOverride)) {
    }

    if (!mIsOverride)
        return;
    // draw the dropdowns for the keys

    ImGui::Combo("Key 1", (int*)(&mKey1), KeyNames, hk::util::arraySize(KeyNames));
    ImGui::Combo("Key 2", (int*)(&mKey2), KeyNames, hk::util::arraySize(KeyNames));
}

void CategoryHotkeyBase::hotkeyFunc() {};

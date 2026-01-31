#pragma once

#include "devgui/categories/hotkeys/CategoryHotkeyBase.h"

typedef int ImGuiComboFlags;  // Forward declaration

class CategoryDupKill : public CategoryHotkeyBase {
public:
    CategoryDupKill(const char* catName, const char* catDesc, sead::Heap* heap);

private:
    void hotkeyFunc() override;
};

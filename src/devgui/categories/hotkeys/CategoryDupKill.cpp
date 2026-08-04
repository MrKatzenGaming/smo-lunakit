#include "devgui/categories/hotkeys/CategoryDupKill.h"

#include "helpers/GetHelper.h"
#include "helpers/PlayerHelper.h"

CategoryDupKill::CategoryDupKill(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryHotkeyBase(catName, catDesc, heap) {}

void CategoryDupKill::hotkeyFunc() {
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();

    // check if player exists
    if (!player)
        return;

    PlayerHelper::killPlayer(player);
}

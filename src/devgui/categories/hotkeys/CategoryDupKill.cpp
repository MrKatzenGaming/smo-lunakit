#include "devgui/categories/hotkeys/CategoryDupKill.h"

#include "helpers/GetHelper.h"
#include "helpers/PlayerHelper.h"

CategoryDupKill::CategoryDupKill(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryHotkeyBase(catName, catDesc, heap) {}

void CategoryDupKill::hotkeyFunc() {
    HakoniwaSequence* gameSeq = tryGetHakoniwaSequence();
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa(gameSeq);

    // check if player exists
    if (!player)
        return;

    PlayerHelper::killPlayer(player);
}

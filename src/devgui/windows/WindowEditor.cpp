#include "devgui/windows/WindowEditor.h"

#include "devgui/DevGuiManager.h"
#include "devgui/categories/edit/CategoryCoins.h"
#include "devgui/categories/edit/CategoryHealth.h"
#include "devgui/categories/edit/CategoryHomeShip.h"
#include "devgui/categories/edit/CategoryJump.h"
#include "devgui/categories/edit/CategoryOutfit.h"
#include "devgui/categories/edit/CategorySpeed.h"
#include "devgui/categories/edit/CategoryWarp.h"

WindowEditor::WindowEditor(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {
    createCategory<CategoryHealth>("HP", "Edit the player's health");
    createCategory<CategoryCoins>("Coins", "Edit the player's coin count");
    createCategory<CategorySpeed>("Speed", "Edit the player's speed");
    createCategory<CategoryJump>("Jump", "Edit the player's jump height");
    createCategory<CategoryWarp>("Warp", "Save warp positions for player");
    createCategory<CategoryOutfit>("Outfit", "Change player's outfit quickly");
    createCategory<CategoryHomeShip>("Ship", "Edit Odyssey ship progression");
}

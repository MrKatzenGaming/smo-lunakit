#include "devgui/DevGuiManager.h"
#include "WindowDupHotkeys.h"

#include "program/devgui/categories/hotkeys/CategoryDupKill.h"
#include "program/devgui/categories/hotkeys/CategoryReloadScene.h"


WindowDupHotkeys::WindowDupHotkeys(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault)
{
    createCategory<CategoryDupKill>("Kill", "Kill Mario");
    createCategory<CategoryReloadScene>("Reload Scene", "Reload the scene");

}
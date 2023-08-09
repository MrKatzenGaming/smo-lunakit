#include "devgui/DevGuiManager.h"
#include "WindowDupHotkeys.h"

#include "program/devgui/categories/hotkeys/CategoryDupKill.h"
#include "program/devgui/categories/hotkeys/CategoryReloadScene.h"
#include "program/devgui/categories/hotkeys/CategorySetHP.h"


WindowDupHotkeys::WindowDupHotkeys(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault)
{
    createCategory<CategoryDupKill>("Kill", "Kill Mario");
    createCategory<CategoryReloadScene>("Reload Scene", "Reload the scene");
    createCategory<CategorySetHP>("Set HP", "Set Mario's HP");

}
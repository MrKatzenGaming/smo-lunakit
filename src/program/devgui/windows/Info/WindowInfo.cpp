#include "devgui/DevGuiManager.h"
#include "WindowInfo.h"

#include "program/devgui/categories/info/CategoryInfSequence.h"
#include "program/devgui/categories/info/CategoryInfScene.h"
#include "program/devgui/categories/info/CategoryInfPlayer.h"
#include "program/devgui/categories/info/CategoryInfCappy.h"
#include "program/devgui/categories/info/CategoryInfStats.h"

WindowInfo::WindowInfo(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault)
{
    createCategory<CategoryInfSequence>("Sequence", "Sequence Information");
    createCategory<CategoryInfScene>("Scene", "Scene Information");
    createCategory<CategoryInfPlayer>("Player", "Player Information");
    createCategory<CategoryInfCappy>("Cappy", "Cappy Information");
    createCategory<CategoryInfStats>("Stats", "Stats Information");

}
#include "devgui/windows/Info/WindowInfo.h"

#include "devgui/DevGuiManager.h"
#include "devgui/categories/info/CategoryInfCappy.h"
#include "devgui/categories/info/CategoryInfPlayer.h"
#include "devgui/categories/info/CategoryInfScene.h"
#include "devgui/categories/info/CategoryInfSequence.h"
#include "devgui/categories/info/CategoryInfStats.h"

WindowInfo::WindowInfo(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {
    createCategory<CategoryInfSequence>("Sequence", "Sequence Information");
    createCategory<CategoryInfScene>("Scene", "Scene Information");
    createCategory<CategoryInfPlayer>("Player", "Player Information");
    createCategory<CategoryInfCappy>("Cappy", "Cappy Information");
    createCategory<CategoryInfStats>("Stats", "Stats Information");
}

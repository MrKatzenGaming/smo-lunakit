#pragma once

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameactorBrowse2 = "Actor Browser 2";

class CategoryActorList;
class CategoryActorInspectors;

class WindowActorBrowse2 : public WindowBase {
public:
    WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;

    CategoryActorList* getListCategory() const { return mCatList; }
    CategoryActorInspectors* getInspectorsCategory() const { return mCatInspectors; }

private:
    CategoryActorList* mCatList;
    CategoryActorInspectors* mCatInspectors;
};

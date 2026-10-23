#include "devgui/windows/WindowActorBrowse2.h"

#include "al/Library/LiveActor/LiveActorKit.h"

#include <cxxabi.h>

#include "devgui/DevGuiManager.h"
#include "devgui/categories/actor/CategoryActorInspectors.h"
#include "devgui/categories/actor/CategoryActorList.h"
#include "devgui/windows/WindowBase.h"
#include "helpers/GetHelper.h"

WindowActorBrowse2::WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault) {
    mCatList = createCategory<CategoryActorList>("Actors", "Actor List");
    mCatInspectors = createCategory<CategoryActorInspectors>("Inspectors", "Inspector List");
}

void WindowActorBrowse2::updateWin() {
    WindowBase::updateWin();

    al::Scene* scene = tryGetScene();
    if (!scene) {
        mCatList->resetScene();
        mCatInspectors->resetScene();
        return;
    }

    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (!kit) {
        mCatList->resetScene();
        mCatInspectors->resetScene();
        return;
    }

    al::LiveActorGroup* group = kit->mLiveActorGroupAllActors;
    if (!group) {
        mCatList->resetScene();
        mCatInspectors->resetScene();
        return;
    }
}

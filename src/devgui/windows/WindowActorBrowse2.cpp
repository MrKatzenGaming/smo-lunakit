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
        resetScene();
        return;
    }

    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (!kit) {
        resetScene();
        return;
    }

    al::LiveActorGroup* group = kit->mLiveActorGroupAllActors;
    if (!group) {
        resetScene();
        return;
    }

    mGeneration++;
}

void WindowActorBrowse2::resetScene() {
    mCatList->resetScene();
    mCatInspectors->resetScene();

    mProviderStorage.forEach([](auto keyImpl, InfoProviderBase* provider) { delete provider; });
    mProviderStorage.clear();

    mGeneration = 0;
}

bool WindowActorBrowse2::addProvider(const void* key, InfoProviderBase* provider) {
    if (mProviderStorage.mSize >= mProviderStorage.mCapacity) {
        // Storage is full, perform garbage collection pass
        mProviderStorage.MapImpl::forEach([this](auto* base_node) {
            auto* node = static_cast<ProviderStorage::Node*>(base_node);
            if (node->value()->getGeneration() < getGeneration())
                node->erase_();
        });

        if (mProviderStorage.mSize >= mProviderStorage.mCapacity)
            return false;  // Still no capacity
    }

    mProviderStorage.insert(key, provider);
    return true;
}

InfoProviderBase* WindowActorBrowse2::getProvider(const void* key) const {
    auto node = mProviderStorage.find(key);
    if (node)
        return node->value();
    else
        return nullptr;
}

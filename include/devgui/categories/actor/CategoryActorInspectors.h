#pragma once

#include "sead/container/seadPtrArray.h"
#include "sead/container/seadTreeMap.h"

#include "devgui/categories/CategoryBase.h"
#include "info/InfoProviderBase.h"

class CategoryActorInspectors : public CategoryBase {
public:
    CategoryActorInspectors(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCat() override;
    void updateCatDisplay() override;
    void resetScene();

    void addProvider(const void* key, InfoProviderBase* provider);
    void removeProvider(const void* key);
    InfoProviderBase* getProvider(const void* key) const;

private:
    sead::FixedTreeMap<const void*, InfoProviderBase*, 128> mProviders;
    const void* mPendingDelete = nullptr;  // Cannot remove while iterating
};

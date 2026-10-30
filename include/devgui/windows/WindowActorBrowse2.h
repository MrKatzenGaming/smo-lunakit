#pragma once

#include "sead/container/seadTreeMap.h"

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameActorBrowse2 = "Actor Browser 2";

class CategoryActorList;
class CategoryActorInspectors;
class InfoProviderBase;

class WindowActorBrowse2 : public WindowBase {
public:
    WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;

    CategoryActorList* getListCategory() const { return mCatList; }
    CategoryActorInspectors* getInspectorsCategory() const { return mCatInspectors; }

    bool addProvider(const void* key, InfoProviderBase* provider);
    InfoProviderBase* getProvider(const void* key) const;

    u32 getGeneration() const { return mGeneration; }

private:
    void resetScene();

    CategoryActorList* mCatList;
    CategoryActorInspectors* mCatInspectors;

    using ProviderStorage = sead::FixedTreeMap<const void*, InfoProviderBase*, 1024>;
    ProviderStorage mProviderStorage;
    u32 mGeneration = 0;
};

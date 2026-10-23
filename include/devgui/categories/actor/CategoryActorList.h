#pragma once

#include "sead/container/seadPtrArray.h"

#include "al/Library/LiveActor/LiveActorGroup.h"

#include "devgui/categories/CategoryBase.h"
#include "imgui.h"

class CategoryActorList : public CategoryBase {
public:
    CategoryActorList(const char* catName, const char* catDesc, sead::Heap* heap);

    void updateCat() override;
    void updateCatDisplay() override;
    void resetScene();

private:
    void displayActorList(al::LiveActorGroup* group);
    void displayActorListEntry(al::LiveActor* actor, ImGuiListClipper& clipper);
    void regenerateFilerActorGroup(const al::LiveActorGroup* group);
    bool checkActorMatchesFilter(const al::LiveActor* actor) const;

    void displayActorInspector(al::LiveActor* actor);

    al::LiveActor* mSelectedActor = nullptr;
    sead::FixedPtrArray<al::LiveActor, 32> mPinnedActors;

    const char* mSearchText;
    ImGuiTextFilter mSearchFilter;
    al::LiveActorGroup* mFilterActorGroup;

    static constexpr f32 cMaxPickDistance = 10000.0f;
    f32 mPickRadius = 100.0f;
    s32 mPickHitNum = 0;
    s32 mPickHitIdx = 0;

    bool mIsLayoutHorizontal;
    bool mIsPopupKeyboardOpen = false;
    bool mIsPickingActor = false;
    // bool mShouldRenderAllSensors = true;
    bool mShouldScrollToSelectedActor = false;
    bool mDidChangePickIdx = false;
};

#pragma once

#include "sead/container/seadPtrArray.h"

#include "al/Library/Camera/ActorCameraTarget.h"
#include "al/Library/HitSensor/HitSensorKeeper.h"
#include "al/Library/LiveActor/LiveActor.h"
#include "al/Library/LiveActor/LiveActorGroup.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "devgui/windows/ActorBrowse2/ActorBrowseInspector.h"
#include "devgui/windows/WindowBase.h"
#include "imgui.h"

__attribute__((used)) static const char* windowNameactorBrowse2 = "Actor Browser 2";

class WindowActorBrowse2 : public WindowBase {
    friend class ActorBrowseInspectorBase;

public:
    WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;

private:
    static constexpr f32 cHeaderSize = 0.0f;  // TODO

    void resetScene();

    void displayActorList(al::LiveActorGroup* group);
    void displayActorListEntry(al::LiveActor* actor, ImGuiListClipper& clipper);
    void regenerateFilerActorGroup(const al::LiveActorGroup* group);
    bool checkActorMatchesFilter(const al::LiveActor* actor) const;

    void displayActorInspector(al::Scene* scene, al::LiveActor* actor);

    ActorBrowseInspectorBase mActorInspectorDefault;
    ActorBrowseInspectorFukuwaraiFaceParts mActorInspectorFukuwaraiFaceParts;
    ActorBrowseInspectorFukuwaraiWatcher mActorInspectorFukuwaraiWatcher;
    ActorBrowseInspectorGrowPlantSeed mActorInspectorGrowPlantSeed;

    al::LiveActor* mSelectedActor = nullptr;
    al::HitSensor* mHoveredSensor = nullptr;
    sead::FixedPtrArray<al::LiveActor, 32> mPinnedActors;
    sead::FixedPtrArray<al::HitSensor, 64> mRenderedSensors;

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
    bool mShouldRenderAllSensors = true;
    bool mShouldScrollToSelectedActor = false;
    bool mDidChangePickIdx = false;
};

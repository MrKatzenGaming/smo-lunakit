#pragma once

#include "Library/HitSensor/HitSensorKeeper.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorFlag.h"
#include "Library/Nerve/NerveKeeper.h"
#include "al/Library/Camera/ActorCameraTarget.h"
#include "al/Library/LiveActor/LiveActorGroup.h"

#include "devgui/windows/WindowBase.h"
#include "imgui.h"

__attribute__((used)) static const char* windowNameactorBrowse2 = "Actor Browser 2";

class WindowActorBrowse2 : public WindowBase {
public:
    WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;

private:
    static constexpr f32 cHeaderSize = 0.0f;  // TODO

    void displayActorList(al::LiveActorGroup* group);
    void displayActorListEntry(al::LiveActor* actor, ImGuiListClipper& clipper);
    void regenerateFilerActorGroup(const al::LiveActorGroup* group);
    bool checkActorMatchesFilter(const al::LiveActor* actor) const;

    void displayActorInspector(al::LiveActor* actor);

    void displayActorInspectorTreeName(al::LiveActor* actor);
    void displayActorInspectorTreePose(al::ActorPoseKeeperBase* poseKeep);
    void displayActorInspectorTreeFlags(al::LiveActorFlag* flags);
    void displayActorInspectorTreeNrv(al::NerveKeeper* nrvKeep);
    void displayActorInspectorTreeSensor(al::HitSensorKeeper* sensorKeep);

    al::LiveActor* mSelectedActor = nullptr;

    const char* mSearchText;
    ImGuiTextFilter mSearchFilter;
    al::LiveActorGroup* mFilterActorGroup;
    bool mKeyboardOpen = false;
};

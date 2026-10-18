#pragma once

#include "Library/LiveActor/LiveActor.h"
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

    void drawActor(al::LiveActor* actor, ImGuiListClipper& clipper);
    void drawActorList(al::LiveActorGroup* group);

    void regenerateFilerActorGroup(const al::LiveActorGroup* group);
    bool checkActorMatchesFilter(const al::LiveActor* actor) const;

    al::LiveActor* mSelectedActor = nullptr;

    const char* mSearchText;
    ImGuiTextFilter mSearchFilter;
    al::LiveActorGroup* mFilterActorGroup;
    bool mKeyboardOpen = false;
};

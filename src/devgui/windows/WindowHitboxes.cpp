#include "devgui/windows/WindowHitboxes.h"

#include "hk/hook/Trampoline.h"

#include "sead/gfx/seadCamera.h"
#include "sead/heap/seadDisposer.h"
#include "sead/math/seadVector.h"

#include "al/Library/Camera/CameraDirector.h"
#include "al/Library/Camera/CameraPoseUpdater.h"
#include "al/Library/Camera/CameraPoserSceneInfo.h"
#include "al/Library/Camera/CameraRequestParamHolder.h"
#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/Camera/CameraViewInfo.h"
#include "al/Library/Camera/ICameraInput.h"
#include "al/Library/Camera/SceneCameraInfo.h"
#include "al/Library/Controller/InputFunction.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Library/Player/PlayerUtil.h"
#include "al/Library/Projection/Projection.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Scene/SceneUtil.h"
#include "al/Library/Sequence/Sequence.h"
#include "al/Library/System/GameSystemInfo.h"

#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameSystem.h"

#include "devgui/DevGuiManager.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"

WindowHitboxes::WindowHitboxes(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {
    mCollisionDrawer.genCube(sead::Vector3f(100.0f, 100.0f, 100.0f), 100.0f);
}

static const al::Scene* getScene(GameSystem* system) {
    const auto* sequence = system->mSequence;
    if (!sequence || !sequence->mIsAlive)
        return nullptr;

    const auto* scene = sequence->mCurrentScene;
    if (!scene || !scene->mIsAlive)
        return nullptr;

    return scene;
}

void WindowHitboxes::updateWin() {
    if (!mEnabled)
        return;

    const auto* sequence = GameSystemFunction::getGameSystem()->mSequence;
    if (!sequence || !sequence->mIsAlive)
        return;

    const auto* scene = sequence->mCurrentScene;
    if (!scene || !scene->mIsAlive)
        return;

    agl::DrawContext* context = sequence->getDrawInfo()->drawContext;
    mCollisionDrawer.draw(context, scene);
}

bool WindowHitboxes::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    ImGui::Checkbox("Enable", &mEnabled);

    return true;
}

#include "devgui/windows/WindowFreecam.h"

#include "hk/hook/Trampoline.h"

#include "sead/gfx/seadCamera.h"

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

#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameSystem.h"

#include "devgui/DevGuiManager.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "math/seadVector.h"

WindowFreecam::WindowFreecam(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {}

static const al::Scene* getScene(GameSystem* system) {
    const auto* sequence = system->mSequence;
    if (!sequence || !sequence->mIsAlive)
        return nullptr;

    const auto* scene = sequence->mCurrentScene;
    if (!scene || !scene->mIsAlive)
        return nullptr;

    return scene;
}

void WindowFreecam::updateWin() {
    if (!mEnabled)
        return;

    auto* scene = getScene(GameSystemFunction::getGameSystem());
    if (!scene)
        return;

    auto& cam = scene->getCameraDirector()->getPoseUpdater(0)->mLookAtCamera;

    switch (mMode) {
    case Mode::FirstPerson: {
        // Position
        sead::Vector2f leftStick = InputHelper::getLeftStick();

        sead::Vector3f plane = {0.0f, 0.0f, 1.0f};
        al::rotateVectorDegreeX(&plane, mRotation.y);
        al::rotateVectorDegreeY(&plane, mRotation.x);
        plane.y = 0.0f;

        sead::Vector3f right;
        right.setCross(plane, sead::Vector3f::ey);

        mPosition += right * leftStick.x * mMoveSpeed;
        mPosition += plane * leftStick.y * mMoveSpeed;
        if (InputHelper::isHoldZL())
            mPosition.y -= mMoveSpeed;
        if (InputHelper::isHoldZR())
            mPosition.y += mMoveSpeed;

        break;
    }
    case Mode::OrbitPlayer: {
        // Zoom
        if (InputHelper::isHoldL())
            mZoom -= mZoomSpeed;
        if (InputHelper::isHoldR())
            mZoom += mZoomSpeed;

        al::PlayerHolder* pHolder = al::getScenePlayerHolder(scene);
        if (!pHolder)
            break;

        PlayerActorBase* playerBase = static_cast<PlayerActorBase*>(al::tryGetPlayerActor(pHolder, 0));
        if (!playerBase || typeid(*playerBase).name() != typeid(PlayerActorHakoniwa).name())
            break;

        PlayerActorHakoniwa* player = static_cast<PlayerActorHakoniwa*>(playerBase);
        mPosition = player->mPoseKeeper->mTrans;
        break;
    }
    }

    // Rotation
    sead::Vector2f rightStick = InputHelper::getRightStick();
    if (al::isNearZero(rightStick.x, 0.3f))
        rightStick.x = 0.0f;
    if (al::isNearZero(rightStick.y, 0.3f))
        rightStick.y = 0.0f;

    mRotation.x = al::wrapAngle(mRotation.x - rightStick.x * mRotateSpeed);
    mRotation.y = al::clamp(mRotation.y - rightStick.y * mRotateSpeed, -89.9f, 89.9f);
}

bool WindowFreecam::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    ImGui::Checkbox("Enable", &mEnabled);
    if (ImGui::BeginCombo("Mode", modeNames[(s32)mMode])) {
        for (u8 i = 0; i <= (u8)Mode::OrbitPlayer; i++) {
            bool selected = mMode == (Mode)i;
            if (ImGui::Selectable(modeNames[i], selected))
                mMode = (Mode)i;

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    switch (mMode) {
    case Mode::FirstPerson:
        ImGui::DragFloat("Move Speed", &mMoveSpeed, 1.0f, 0.0f, 1000.0f, "%.0f/f");
        ImGui::DragFloat("Rotate Speed", &mRotateSpeed, 0.1f, 0.1f, 30.0f, "%.1f°/f");

        ImGui::Separator();

        ImGui::DragFloat3("Position", (f32*)&mPosition, 1.0f, 0.f, 0.0f, "%.0f");
        ImGui::DragFloat2("Rotation", (f32*)&mRotation, 0.1f, -180.0f, 180.0f, "%.1f°");
        break;
    case Mode::OrbitPlayer:
        ImGui::DragFloat("Rotate Speed", &mRotateSpeed, 0.1f, 0.1f, 30.0f, "%.1f°/f");
        ImGui::DragFloat("Zoom Speed", &mZoomSpeed, 1.0f, 0.0f, 100.0f, "%.0f/f");

        ImGui::Separator();

        ImGui::DragFloat2("Rotation", (f32*)&mRotation, 0.1f, -180.0f, 180.0f, "%.1f°");
        ImGui::DragFloat("Zoom", (f32*)&mZoom, 1.0f, 10.0f, 1000.0f, "%.0f");
        break;
    }

    ImGui::Separator();

    ImGui::DragFloat("Near Clipping", (f32*)&mNearClip, 1.0f, 1.0f, 1.0f, "%.0f");
    ImGui::DragFloat("Far Clipping", (f32*)&mFarClip, 100.0f, 10'000.0f, 1'000'000.0f, "%.0f");
    ImGui::DragFloat("Field of View", (f32*)&mFovyDegree, 1.0f, 179.0f, 1.0f, "%.0f°");

    ImGui::Separator();

    if (ImGui::Button("Reset to Active Camera")) {
        sead::Vector3f activePos;
        sead::Vector3f activeAt;
        sead::Vector3f activeUp;
        al::Projection* activeProj;

        if (mEnabled) {
            activePos = mOrigPos;
            activeAt = mOrigAt;
            activeUp = mOrigUp;
            activeProj = &mOrigProj;
        } else {
            auto* scene = getScene(GameSystemFunction::getGameSystem());
            if (scene) {
                auto* poseUpdater = scene->getCameraDirector()->getPoseUpdater(0);

                activePos = poseUpdater->mLookAtCamera.mPos;
                activeAt = poseUpdater->mLookAtCamera.mAt;
                activeUp = poseUpdater->mLookAtCamera.mUp;
                activeProj = poseUpdater->mProjection;
            } else {
                Logger::log("Failed to get scene!\n");
            }
        }

        mPosition = activePos;

        sead::Vector3f forward = activeAt - activePos;
        al::tryNormalizeOrDirZ(&forward);

        mRotation.x = al::wrapAngle(sead::Mathf::rad2deg(sead::Mathf::atan2(forward.x, forward.z)));
        mRotation.y = al::wrapAngle(sead::Mathf::rad2deg(sead::Mathf::asin(-forward.y)));

        mNearClip = activeProj->getNear();
        mFarClip = activeProj->getFar();
        mFovyDegree = sead::Mathf::rad2deg(activeProj->getFovy());
    }

    return true;
}

HkTrampoline drawMainHook = [](TrampolineStatic(), GameSystem* system) -> void {
    WindowFreecam* freecam = DevGuiManager::instance()->getWindow<WindowFreecam>("Freecam");

    if (!freecam->mEnabled) {
        orig(system);
        return;
    }

    auto* scene = getScene(GameSystemFunction::getGameSystem());
    if (!scene) {
        orig(system);
        return;
    }

    auto* poseUpdater = scene->getCameraDirector()->getPoseUpdater(0);

    // Backup
    freecam->mOrigPos = poseUpdater->mLookAtCamera.mPos;
    freecam->mOrigAt = poseUpdater->mLookAtCamera.mAt;
    freecam->mOrigUp = poseUpdater->mLookAtCamera.mUp;
    freecam->mOrigProj.copyFrom(*poseUpdater->mProjection);

    // Apply
    sead::Vector3f forward = {0.0f, 0.0f, 1.0f};
    al::rotateVectorDegreeX(&forward, freecam->mRotation.y);
    al::rotateVectorDegreeY(&forward, freecam->mRotation.x);

    f32 zoom = freecam->mMode == WindowFreecam::Mode::OrbitPlayer ? freecam->mZoom : 0.0f;
    poseUpdater->mLookAtCamera.mPos = freecam->mPosition - forward * zoom;
    poseUpdater->mLookAtCamera.mAt = freecam->mPosition + forward;
    poseUpdater->mLookAtCamera.mUp = sead::Vector3f::ey;
    poseUpdater->mLookAtCamera.updateViewMatrix();

    poseUpdater->mProjection->setProj(freecam->mNearClip, freecam->mFarClip, sead::Mathf::deg2rad(freecam->mFovyDegree), poseUpdater->mAspect);
    poseUpdater->mProjection->setOffset(sead::Vector2f::zero);
    poseUpdater->mProjection->calcMtx();

    orig(system);

    // Restore
    poseUpdater->mLookAtCamera.mPos = freecam->mOrigPos;
    poseUpdater->mLookAtCamera.mAt = freecam->mOrigAt;
    poseUpdater->mLookAtCamera.mUp = freecam->mOrigUp;
    poseUpdater->mLookAtCamera.updateViewMatrix();
    poseUpdater->mProjection->copyFrom(freecam->mOrigProj);
};

void WindowFreecam::installHooks() {
    drawMainHook.installAtSym<"_ZN10GameSystem8drawMainEv">();
    Logger::log("Installed hook!\n");
}

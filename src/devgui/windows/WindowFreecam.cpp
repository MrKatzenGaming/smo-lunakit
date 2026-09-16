#include "devgui/windows/WindowFreecam.h"

#include "hk/hook/Trampoline.h"

#include "Library/Camera/CameraDirector.h"
#include "Library/Camera/CameraPoseUpdater.h"
#include "Library/Camera/CameraPoserSceneInfo.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Camera/CameraViewInfo.h"
#include "Library/Camera/CameraRequestParamHolder.h"
#include "Library/Camera/SceneCameraInfo.h"
#include "Library/Camera/ICameraInput.h"
#include "Library/Controller/InputFunction.h"
#include "Library/LiveActor/LiveActorKit.h"
#include "Library/Math/MathUtil.h"
#include "Library/Scene/Scene.h"
#include "Library/Sequence/Sequence.h"

#include "Sequence/HakoniwaSequence.h"
#include "System/GameSystem.h"
#include "devgui/DevGuiManager.h"
#include "gfx/seadCamera.h"
#include "helpers/InputHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "math/seadVectorFwd.h"

WindowFreecam::WindowFreecam(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {}

static const al::CameraDirector* getCameraDirector(GameSystem* system) {
    const auto* sequence = system->mSequence;
    if (!sequence || !sequence->mIsAlive) {
        return nullptr;
    }

    const auto* scene = sequence->mCurrentScene;
    if (!scene || !scene->mIsAlive) {
        return nullptr;
    }
   
    return scene->getCameraDirector();
}

void WindowFreecam::updateWin() {
    if (!mEnabled) {
        return;
    }

    auto* director = getCameraDirector(GameSystemFunction::getGameSystem());
    if (!director) {
        return;
    }

    auto& cam = director->getPoseUpdater(0)->mLookAtCamera;

    sead::Vector2f leftStick = InputHelper::getLeftStick() * mMoveSpeed;
    sead::Vector2f rightStick = InputHelper::getRightStick() * mRotateSpeed;
   
    sead::Vector3f forward = mFreeAt - mFreePos;
    al::tryNormalizeOrDirZ(&forward);

    sead::Vector3f plane = {forward.x, 0.0f, forward.z};
    al::tryNormalizeOrDirZ(&plane);

    sead::Vector3f right;
    right.setCross(forward, mFreeUp);

    // Movement
    mFreePos += leftStick.y * plane + leftStick.x * right;
    if (InputHelper::isHoldZL()) {
        mFreePos.y -= mMoveSpeed;
    }
    if (InputHelper::isHoldZR()) {
        mFreePos.y += mMoveSpeed;
    }

    if (al::isNearZero(rightStick.x, 0.3f * mRotateSpeed)) {
        rightStick.x = 0.0f;
    }
    if (al::isNearZero(rightStick.y, 0.3f * mRotateSpeed)) {
        rightStick.y = 0.0f;
    }
    
    // Rotation
    al::rotateVectorDegree(&forward, forward, right, rightStick.y);
    al::rotateVectorDegree(&forward, forward, mFreeUp, -rightStick.x);
    mFreeAt = mFreePos + forward;
}

bool WindowFreecam::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    ImGui::Checkbox("Enable", &mEnabled);

    ImGui::Separator();

    ImGui::DragFloat("Move Speed", &mMoveSpeed, 1.0f, 0.0f, 1000.0f, "%.0f units/f");
    ImGui::DragFloat("Rotate Speed", &mRotateSpeed, 0.1f, 0.1f, 30.0f, "%.1f°/f");

    ImGui::Separator();

    sead::Vector3f forward = mFreeAt - mFreePos;
    al::tryNormalizeOrDirZ(&forward);
    
    sead::Vector2f rotation = {
        sead::Mathf::rad2deg(sead::Mathf::atan2(forward.x, forward.z)),
        sead::Mathf::rad2deg(sead::Mathf::asin(-forward.y))
    };
    
    bool needUpdate = false;
    needUpdate |= ImGui::DragFloat3("Position", (f32*)&mFreePos, 1.0f, 0.f, 0.0f, "%.0f");
    needUpdate |= ImGui::DragFloat2("Rotation", (f32*)&rotation, 0.1f, -180.0f, 180.0f, "%.1f°");

    ImGui::Separator();

    if (ImGui::Button("Teleport to Active Camera")) {
        if (mEnabled) {
            mFreePos = mOrigPos;
            mFreeAt = mOrigAt;
            mFreeUp = mOrigUp;
        } else {
            auto* director = getCameraDirector(GameSystemFunction::getGameSystem());
            if (director) {
                auto& cam = director->getPoseUpdater(0)->mLookAtCamera;

                mFreePos = cam.mPos;
                mFreeAt = cam.mAt;
                mFreeUp = cam.mUp;
            } else {
                Logger::log("Failed to get active CameraDirector!\n");
            }
        }
    } else if (needUpdate) {
        forward = {0.0f, 0.0f, 1.0f};
        al::rotateVectorDegreeX(&forward, rotation.y);
        al::rotateVectorDegreeY(&forward, rotation.x);
        
        mFreeAt = mFreePos + forward;
    }

    return true;
}

HkTrampoline drawMainHook = [](TrampolineStatic(), GameSystem* system) -> void {
    WindowFreecam* freecam = DevGuiManager::instance()->getWindow<WindowFreecam>("Freecam");

    if (!freecam->mEnabled) {
        orig(system);
        return;
    }

    auto* director = getCameraDirector(GameSystemFunction::getGameSystem());
    if (!director) {
        orig(system);
        return;
    }

    auto& cam = director->getPoseUpdater(0)->mLookAtCamera;

    // Backup
    freecam->mOrigPos = cam.mPos;
    freecam->mOrigAt = cam.mAt;
    freecam->mOrigUp = cam.mUp;
    
    // Apply
    cam.mPos = freecam->mFreePos;
    cam.mAt = freecam->mFreeAt;
    cam.mUp = freecam->mFreeUp;
    cam.updateViewMatrix();
    
    orig(system);

    // Restore
    cam.mPos = freecam->mOrigPos;
    cam.mAt = freecam->mOrigAt;
    cam.mUp = freecam->mOrigUp;
    cam.updateViewMatrix();
};

void WindowFreecam::installHooks() {
    drawMainHook.installAtSym<"_ZN10GameSystem8drawMainEv">();
    Logger::log("Installed hook!\n");
}


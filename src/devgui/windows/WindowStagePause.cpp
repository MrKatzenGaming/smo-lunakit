#include "devgui/windows/WindowStagePause.h"

#include "hk/hook/Trampoline.h"

#include "sead/controller/seadControllerMgr.h"
#include "sead/heap/seadHeapMgr.h"

#include "al/Library/Controller/InputFunction.h"
#include "al/Library/Controller/JoyPadAccelerometerAddon.h"
#include "al/Library/Controller/NpadController.h"
#include "al/Library/Nerve/NerveUtil.h"

#include "custom/al/Pad/PadGyroAddon.h"
#include "custom/game/Scene/StageScene.h"

#include "devgui/DevGuiHooks.h"
#include "devgui/DevGuiManager.h"
#include "helpers/GetHelper.h"
#include "imgui.h"
#include "stage-pause/StageSceneStateStagePause.h"

WindowStagePause::WindowStagePause(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault) {}

bool WindowStagePause::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    if (ImGui::Button("Toggle Scene Pause"))
        tryTogglePause();
    if (ImGui::Button("Advance Frame") && mIsStagePaused)
        mIsAdvanceFrame = true;
    ImGui::Checkbox("Pause Music?", &mIsPauseAudio);

    ImGui::Checkbox("Enable Buffering Inputs", &mIsUseBuffer);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Breaks inputs if not paused");

    ImGui::Checkbox("Camera during Stage pause", &mCamDuringPause);
    return true;
}

void WindowStagePause::runFrame() {
    if (!mIsUseBuffer)
        return;
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
    auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(0));
    controller->mSixAxisSensorCount = 2;  // number of accelerometers for joycons
    auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 0);
    auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 1);
    auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 0);
    auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 1);
    // do all the controller shit with frame
    controller->mLeftStick = {0, 0};
    controller->mRightStick = {0, 0};
    // accelLeft->mAcceleration = frame.mLeftAccel;
    // accelRight->mAcceleration = frame.mRightAccel;
    // gyroLeft->mDirection = frame.mLeftGyro.mDirection;
    // gyroRight->mDirection = frame.mRightGyro.mDirection;
    // gyroLeft->mAngularVel = frame.mLeftGyro.mAngularV;
    // gyroRight->mAngularVel = frame.mRightGyro.mAngularV;

    controller->mPadTrig = 0;
    controller->mPadRelease = 0;
    controller->mPadHold = 0;

    auto* controller2 = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(1));
    controller->mSixAxisSensorCount = 2;  // number of accelerometers for joycons
    auto* accelLeft2 = (al::JoyPadAccelerometerAddon*)controller2->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 0);
    auto* accelRight2 = (al::JoyPadAccelerometerAddon*)controller2->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 1);
    auto* gyroLeft2 = (al::PadGyroAddon*)controller2->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 0);
    auto* gyroRight2 = (al::PadGyroAddon*)controller2->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 1);

    controller2->mLeftStick = {0, 0};
    controller2->mRightStick = {0, 0};
    // accelLeft2->mAcceleration = frame.mLeftAccel;
    // accelRight2->mAcceleration = frame.mRightAccel;
    // gyroLeft2->mDirection = frame.mLeftGyro.mDirection;
    // gyroRight2->mDirection = frame.mRightGyro.mDirection;
    // gyroLeft2->mAngularVel = frame.mLeftGyro.mAngularV;
    // gyroRight2->mAngularVel = frame.mRightGyro.mAngularV;

    controller2->mPadTrig = 0;
    controller2->mPadRelease = 0;
    controller2->mPadHold = 0;
}

void WindowStagePause::updateWin() {
    WindowBase::updateWin();
    tryAdvanceFrame();
    runFrame();
    if (mShouldPause && !mIsPausing) {
        mShouldPause = false;
        tryTogglePause();
    }
}

void WindowStagePause::tryTogglePause() {
    mIsStagePaused ? tryUnpauseScene() : tryPauseScene();
    mIsStagePaused = !mIsStagePaused;
    mIsPausing = !mIsPausing;
}

bool WindowStagePause::tryPauseScene() {
    if (mIsStagePaused)
        return false;

    StageScene* stage = tryGetStageScene();
    if (!stage) {
        mIsStagePaused = false;
        return false;
    }

    if (!stage->getNerveKeeper()) {
        mIsStagePaused = false;
        return false;
    }

    mPreviousNerveKeeper = *stage->getNerveKeeper();
    auto state = stage->getNerveKeeper()->mStateCtrl;
    mPreviousState = state->mCurrentState;
    stage->mStatePause->mIsPauseAudio = mIsPauseAudio;
    stage->mStatePause->mIsPaused = true;
    stage->getNerveKeeper()->mCurrentNerve = &StageSceneNrvStagePause::sInstance;
    state->startState(&StageSceneNrvStagePause::sInstance);

    return true;
}

bool WindowStagePause::tryUnpauseScene() {
    if (!mIsStagePaused)
        return false;

    StageScene* stage = tryGetStageScene();
    if (!stage)
        return false;

    if (!stage->getNerveKeeper())
        return false;
    stage->mStatePause->mIsPaused = false;
    stage->getNerveKeeper()->mStateCtrl->tryEndCurrentState();
    *stage->getNerveKeeper() = mPreviousNerveKeeper;
    stage->getNerveKeeper()->mStateCtrl->mCurrentState = mPreviousState;

    return true;
}

bool WindowStagePause::tryAdvanceFrame() {
    if (mIsAdvanceFrame && !mIsStagePaused) {
        tryPauseScene();
        mIsAdvanceFrame = false;
        mIsStagePaused = true;
    }
    if (mIsAdvanceFrame && mIsStagePaused) {
        tryUnpauseScene();
        mIsStagePaused = false;
    }
    return true;
}

HkTrampoline StageSceneInitHook = [](TrampolineStatic(), StageScene* thisPtr, al::SceneInitInfo* initInfo) -> void {
    orig(thisPtr, initInfo);
    thisPtr->mStatePause = new StageSceneStateStagePause("StagePause", thisPtr, thisPtr->mAudioSystemPauseController);
    if (thisPtr->mStatePause)
        al::initNerveState(thisPtr, thisPtr->mStatePause, &StageSceneNrvStagePause::sInstance, "StagePause");
};

HkTrampoline ShadowUpdateHook = [](TrampolineStatic(), al::ShadowKeeper* thisPtr) -> void {
    WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
    if (win && win->getStagePaused())
        return;

    orig(thisPtr);
};

void DevGuiHooks::exlSetupStageSceneHooks() {
    StageSceneInitHook.installAtSym<"_ZN10StageScene4initERKN2al13SceneInitInfoE">();
    ShadowUpdateHook.installAtSym<"_ZN2al18ShadowMaskDirector16updateShadowMaskEv">();

    hk::hook::a64::assemble<"mov x0 #0x4c8">().installAtSym<"$StagePauseHook1">();
    hk::hook::a64::assemble<"mov x0 #0x4c8">().installAtSym<"$StagePauseHook2">();
}

#include "smo-tas/TAS.h"

#include "nn/fs/fs_directories.h"
#include "nn/fs/fs_files.h"

#include "sead/controller/seadControllerMgr.h"
#include "sead/heap/seadHeapMgr.h"

#include "al/Library/Base/StringUtil.h"
#include "al/Library/Controller/InputFunction.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Nerve/NerveSetupUtil.h"
#include "al/Library/Nerve/NerveUtil.h"

#include "game/MapObj/ChangeStageInfo.h"
#include "game/Player/PlayerActorBase.h"
#include "game/Scene/StageScene.h"
#include "game/System/GameDataHolder.h"
#include "game/System/GameDataHolderAccessor.h"
#include "game/System/GameSystem.h"
#include "game/Util/StageInputFunction.h"

#include "custom/al/Pad/JoyPadAccelerometerAddon.h"
#include "custom/al/Pad/NpadController.h"
#include "custom/al/Pad/PadGyroAddon.h"

#include <typeinfo>

#include "devgui/DevGuiManager.h"
#include "ghost/GhostManager.h"
#include "logger/Logger.hpp"
#include "math/seadVectorFwd.h"

namespace {
NERVE_IMPL(TAS, Update);
NERVE_IMPL(TAS, Wait);
NERVE_IMPL(TAS, WaitUpdate);
NERVE_IMPL(TAS, Record);

NERVES_MAKE_STRUCT(TAS, Update, Wait, WaitUpdate, Record)

}  // namespace

SEAD_SINGLETON_DISPOSER_IMPL(TAS);

TAS::TAS() : al::NerveExecutor("TAS") {
    initNerve(&NrvTAS.Wait, 0);
    nn::fs::CreateDirectory("sd:/smo/tas");
    nn::fs::CreateDirectory(TAS_SCRIPTPATH);

    updateDir();
}

TAS::~TAS() = default;

void TAS::updateDir() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    nn::fs::DirectoryHandle handle = {};
    nn::Result r = nn::fs::OpenDirectory(&handle, TAS_SCRIPTPATH, nn::fs::OpenDirectoryMode_File);
    if (r.IsFailure())
        return;
    s64 entryCount = 0;
    r = nn::fs::GetDirectoryEntryCount(&entryCount, handle);
    if (r.IsFailure()) {
        nn::fs::CloseDirectory(handle);
        return;
    }
    auto* entryBuffer = new nn::fs::DirectoryEntry[entryCount];
    r = nn::fs::ReadDirectory(&entryCount, entryBuffer, handle, entryCount);
    nn::fs::CloseDirectory(handle);
    if (r.IsFailure()) {
        delete[] entryBuffer;
        return;
    }
    delete[] mEntries;
    mEntries = entryBuffer;
    mEntryCount = entryCount;
}

bool TAS::tryStartScript() {
    if (tryLoadScript()) {
        startScript();
        return true;
    }
    return false;
}

bool TAS::tryLoadScript() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    endScript();
    updateDir();
    bool isEntryExist = false;
    for (int i = 0; i < mEntryCount; i++) {
        if (al::isEqualString(mEntries[i].mName, mLoadedEntry.mName)) {
            mLoadedEntry = mEntries[i];
            isEntryExist = true;
        }
    }
    if (!isEntryExist)
        return false;
    sead::FormatFixedSafeString<256> scriptPath(TAS_SCRIPTPATH "/%s", mLoadedEntry.mName);
    nn::fs::FileHandle handle;
    nn::Result r = nn::fs::OpenFile(&handle, scriptPath.cstr(), nn::fs::OpenMode::OpenMode_Read);
    if (r.IsFailure())
        return false;
    mScript = (Script*)new u8[mLoadedEntry.mFileSize];
    r = nn::fs::ReadFile(handle, 0, mScript, mLoadedEntry.mFileSize);
    nn::fs::CloseFile(handle);
    if (r.IsFailure()) {
        endScript();
        return false;
    }
    if (mScript->mMagic != Script::magic) {
        endScript();
        return false;
    }
    return true;
}

void TAS::startScript() {
    bool isWait = false;

    // check if script uses 2-player mode
    if (mScript->mIsTwoPlayer != rs::isSeparatePlay(mScene)) {
        al::GamePadSystem* gamePadSystem = GameSystemFunction::getGameSystem()->mGamePadSystem;
        if (mScript->mIsTwoPlayer) {
            if (!ControllerAppletFunction::connectControllerSeparatePlay(gamePadSystem))
                return;
            rs::changeSeparatePlayMode(mScene, true);
            isWait = true;
        } else {
            if (!ControllerAppletFunction::connectControllerSinglePlay(gamePadSystem))
                return;
            rs::changeSeparatePlayMode(mScene, false);
            isWait = true;
        }
    }

    // check if script metadata has StageName and scenario
    GameDataHolderAccessor accessor(mScene);
    if (!al::isEqualString(mScript->mChangeStageName, "")) {
        ChangeStageInfo info(accessor.mData, mScript->mChangeStageId, mScript->mChangeStageName, false, mScript->mScenarioNo,
                             ChangeStageInfo::NO_SUB_SCENARIO);
        accessor.mData->changeNextStage(&info, 0);
        isWait = true;
    }

    // if stage and/or 2p, wait for 1 frame before starting script
    if (isWait)
        al::setNerve(this, &NrvTAS.WaitUpdate);
    else
        al::setNerve(this, &NrvTAS.Update);
}

void TAS::endScript() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    al::setNerve(this, &NrvTAS.Wait);
    mFrameIndex = 0;
    delete[] mScript;
}

void TAS::applyFrame(InputFrame& frame) {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
    auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(frame.mSecondPlayer));
    controller->mPadAccelerationDeviceNum = 2;  // number of accelerometers for joycons
    auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 0);
    auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 1);
    auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 0);
    auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 1);
    // do all the controller shit with frame
    controller->mLeftStick = frame.mLeftStick;
    controller->mRightStick = frame.mRightStick;
    accelLeft->mAcceleration = frame.mLeftAccel;
    accelRight->mAcceleration = frame.mRightAccel;
    gyroLeft->mDirection = frame.mLeftGyro.mDirection;
    gyroRight->mDirection = frame.mRightGyro.mDirection;
    gyroLeft->mAngularVel = frame.mLeftGyro.mAngularV;
    gyroRight->mAngularVel = frame.mRightGyro.mAngularV;

    controller->mPadTrig = frame.mButtons & ~mPrevButtons[frame.mSecondPlayer];
    controller->mPadRelease = frame.mButtons & mPrevButtons[frame.mSecondPlayer];
    controller->mPadHold = frame.mButtons;

    if (controller->isTrig(1 << 7 /*Left Stick*/))
        TAS::instance()->setIsUseAbsoluteJoystick(!TAS::instance()->isUseAbsoluteJoystick());

    // sead::Vector2f stickL = controller->mLeftStick;
    // sead::Vector3f stickL3D = {0, 0, 0};
    // stickL3D.x = stickL.x;
    // stickL3D.z = stickL.y;
    // PlayerActorBase* playerBase = static_cast<PlayerActorBase*>(rs::getPlayerActor(mScene));

    // stickL3D *= *playerBase->getViewMtx();

    // Logger::log("Frame %d(%d)L:  X: %d, Y:%d\n", frame.mStep, frame.mSecondPlayer, stickL3D.x, stickL3D.z);

    // sead::Vector2f stickR = controller->mRightStick;
    // sead::Vector3f stickR3D = {0, 0, 0};
    // stickR3D.x = stickR.x;
    // stickR3D.z = stickR.y;

    // stickR3D *= *playerBase->getViewMtx();

    // Logger::log("Frame %d(%d)R:  X: %d, Y:%d\n", frame.mStep, frame.mSecondPlayer, stickR3D.x, stickR3D.z);
}

void TAS::exeUpdate() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    if (al::isFirstStep(this)) {
        mFrameIndex = 0;
        mPrevButtons[0] = 0;
        mPrevButtons[1] = 0;
        if (al::isEqualString(typeid(*mScene).name(), typeid(StageScene).name())) {
            PlayerActorBase* playerBase = static_cast<PlayerActorBase*>(rs::getPlayerActor(mScene));
            if (playerBase && !(mScript->mStartPosition.x == 0 && mScript->mStartPosition.y == 0 &&
                                mScript->mStartPosition.z == 0)) {  // teleport unless position is (0, 0, 0)
                playerBase->startDemoPuppetable();
                al::setTrans(playerBase, mScript->mStartPosition);
                playerBase->endDemoPuppetable();
            }
        }
    }
    int step = al::getNerveStep(this);

    bool updated[2] = {false, false};

    while (mFrameIndex < mScript->mFrameCount) {
        // Logger::log("Frame Index: %d, Step: %d\n", mFrameIndex, al::getNerveStep(this));
        InputFrame& curFrame = mScript->mFrames[mFrameIndex];
        if (step < curFrame.mStep)
            break;
        mFrameIndex++;  // increment after checking step
        updated[curFrame.mSecondPlayer] = true;
        applyFrame(curFrame);

        mPrevButtons[curFrame.mSecondPlayer] = curFrame.mButtons;
        // Logger::log("Applied frame: %d, step: %d\n", mFrameIndex, curFrame.mStep);
    }

    if (!updated[0]) {
        mPrevButtons[0] = 0;
        InputFrame frame = {.mSecondPlayer = false};
        applyFrame(frame);
    }
    if (!updated[1]) {
        mPrevButtons[1] = 0;
        InputFrame frame = {.mSecondPlayer = true};
        applyFrame(frame);
    }
    if (mFrameIndex >= mScript->mFrameCount) {
        auto* ghostMgr = GhostManager::instance();
        if (ghostMgr->isRecording())
            ghostMgr->setNerveRecordEnd();
        // Logger::log("Ended Script on Step: %d\n", al::getNerveStep(this));
        // al::setNerve(this, &NrvTASWait);
        endScript();
    }
}

void TAS::exeWait() {}

void TAS::exeWaitUpdate() {
    Logger::log("TAS Wait Update\n");
    al::setNerve(this, &NrvTAS.Update);
}

void TAS::exeRecord() {}

bool TAS::isRunning() {
    return al::isNerve(this, &NrvTAS.Update);
}

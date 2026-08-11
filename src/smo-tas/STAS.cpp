#include "smo-tas/STAS.h"

#include "hk/Result.h"
#include "hk/ValueOrResult.h"
#include "hk/hook/Trampoline.h"
#include "hk/prim/traits/Integer.h"
#include "hk/types.h"

#include "nn/fs/fs_directories.h"

#include "sead/controller/seadControllerMgr.h"
#include "sead/heap/seadHeapMgr.h"
#include "sead/math/seadMatrix.h"
#include "sead/math/seadQuat.h"

#include "Library/Sequence/Sequence.h"
#include "al/Library/Base/StringUtil.h"
#include "al/Library/Controller/InputFunction.h"
#include "al/Library/Controller/JoyPadAccelerometerAddon.h"
#include "al/Library/Controller/NpadController.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Nerve/NerveSetupUtil.h"
#include "al/Library/Nerve/NerveUtil.h"
#include "al/Library/Scene/Scene.h"

#include "game/MapObj/ChangeStageInfo.h"
#include "game/Player/HackCap.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Scene/StageSceneStateOption.h"
#include "game/Scene/StageSceneStatePauseMenu.h"
#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameDataHolder.h"
#include "game/System/GameDataHolderAccessor.h"
#include "game/Util/StageInputFunction.h"

#include "custom/al/Pad/PadGyroAddon.h"

#include <cstring>

#include "devgui/DevGuiManager.h"
#include "devgui/windows/WindowStagePause.h"
#include "ghost/GhostManager.h"
#include "helpers/GetHelper.h"
#include "helpers/fsHelper.h"
#include "logger/Logger.hpp"
#include "smo-tas/Script.h"

namespace {
NERVE_IMPL(STAS, Update);
NERVE_IMPL(STAS, Wait);

NERVES_MAKE_STRUCT(STAS, Update, Wait)

}  // namespace

SEAD_SINGLETON_DISPOSER_IMPL(STAS);

STAS::STAS() : al::NerveExecutor("STAS") {
    initNerve(&NrvSTAS.Wait, 0);
    if (!FsHelper::isDirExist("sd:/smo/tas"))
        nn::fs::CreateDirectory("sd:/smo/tas");
    if (!FsHelper::isDirExist(TAS_SCRIPTPATH))
        nn::fs::CreateDirectory(TAS_SCRIPTPATH);

    updateDir();
}

void STAS::updateDir() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    nn::fs::DirectoryHandle handle = {};
    if (nn::fs::OpenDirectory(&handle, TAS_SCRIPTPATH, nn::fs::OpenDirectoryMode_File).IsFailure())
        return;

    s64 entryCount = 0;
    if (nn::fs::GetDirectoryEntryCount(&entryCount, handle).IsFailure()) {
        nn::fs::CloseDirectory(handle);
        return;
    }

    auto* entryBuffer = new nn::fs::DirectoryEntry[entryCount];
    if (nn::fs::ReadDirectory(&entryCount, entryBuffer, handle, entryCount).IsFailure()) {
        nn::fs::CloseDirectory(handle);
        delete[] entryBuffer;
        return;
    }

    delete[] mEntries;
    mEntries = entryBuffer;
    mEntryCount = entryCount;
}

hk::Result STAS::tryStartScript() {
    if (hk::Result r = tryLoadScript(); r.failed())
        return r;

    // check if script uses 2 - player mode
    al::Sequence* seq = tryGetSequence();
    if (!seq || !seq->mCurrentScene || !seq->mCurrentScene->mIsAlive)
        return hk::ResultFailed();

    if (mScript->is2P() != rs::isSeparatePlay(seq->mCurrentScene)) {
        HakoniwaSequence* s = tryGetHakoniwaSequence();
        if (!s || !s->mCurrentScene || !s->mCurrentScene->mIsAlive)
            return hk::ResultFailed();

        if (mScript->is2P()) {
            if (!ControllerAppletFunction::connectControllerSeparatePlay(s->mGamePadSystem))
                return hk::ResultFailed();
            rs::changeSeparatePlayMode(s->mCurrentScene, true);
        } else {
            if (!ControllerAppletFunction::connectControllerSinglePlay(s->mGamePadSystem))
                return hk::ResultFailed();
            rs::changeSeparatePlayMode(s->mCurrentScene, false);
        }
    }

    al::setNerve(this, &NrvSTAS.Update);

    return hk::ResultSuccess();
}

hk::Result STAS::tryLoadScript() {
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
        return hk::ResultNotFound();
    al::StringTmp<0x100> scriptPath(TAS_SCRIPTPATH "/%s", mLoadedEntry.mName);

    mScript = new Script(DevGuiManager::instance()->getHeap());
    if (mScript->load(scriptPath.cstr()).failed()) {
        endScript();
        return hk::ResultFailed();
    };

    if (mScript->getPlayerCount() > 2) {
        Logger::log("Player Count > 2\n");
        endScript();
        return hk::ResultInvalidArgument();
    }

    return hk::ResultSuccess();
}

void STAS::endScript() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    al::setNerve(this, &NrvSTAS.Wait);
    mCurFrame = -1;
    mNextFrame = -1;
    mPrevButtons.fill(0);
    mIsUseAbsJoystick = false;
    delete mScript;
}

sead::BitFlag32 convertButtonsSTASToSead(sead::BitFlag64 stasPad) {
    sead::BitFlag32 mask = 0;
    for (s32 i = 0; i < 64; i++) {
        if (stasPad.isOnBit(i)) {
            if (i == cSTAS_A)
                mask.setBit(sead::Controller::cPadIdx_A);
            else if (i == cSTAS_B)
                mask.setBit(sead::Controller::cPadIdx_B);
            else if (i == cSTAS_X)
                mask.setBit(sead::Controller::cPadIdx_X);
            else if (i == cSTAS_Y)
                mask.setBit(sead::Controller::cPadIdx_Y);
            else if (i == cSTAS_LeftStick)
                mask.setBit(sead::Controller::cPadIdx_1);
            else if (i == cSTAS_RightStick)
                mask.setBit(sead::Controller::cPadIdx_2);
            else if (i == cSTAS_L)
                mask.setBit(sead::Controller::cPadIdx_L);
            else if (i == cSTAS_R)
                mask.setBit(sead::Controller::cPadIdx_R);
            else if (i == cSTAS_ZL)
                mask.setBit(sead::Controller::cPadIdx_ZL);
            else if (i == cSTAS_ZR)
                mask.setBit(sead::Controller::cPadIdx_ZR);
            else if (i == cSTAS_Plus) {
                mask.setBit(sead::Controller::cPadIdx_Plus);
                mask.setBit(sead::Controller::cPadIdx_Start);
            } else if (i == cSTAS_Minus)
                mask.setBit(sead::Controller::cPadIdx_Minus);
            else if (i == cSTAS_DLeft)
                mask.setBit(sead::Controller::cPadIdx_Left);
            else if (i == cSTAS_DUp)
                mask.setBit(sead::Controller::cPadIdx_Up);
            else if (i == cSTAS_DRight)
                mask.setBit(sead::Controller::cPadIdx_Right);
            else if (i == cSTAS_DDown)
                mask.setBit(sead::Controller::cPadIdx_Down);
        }
    }

    return mask;
}

template <typename T>
T read(u8* src, u64* cursor) {
    T data = *(T*)&src[*cursor];
    *cursor += sizeof(T);
    return data;
}

void STAS::applyCommand(Command* cmd) {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());

    switch (cmd->type) {
    case CommandType::FRAME: {
        mNextFrame = ((CmdFrame*)cmd->data)->frame;
        break;
    }

    case CommandType::CONTROLLER: {
        CmdController c = *(CmdController*)cmd->data;

        if (c.player > 1)
            break;

        sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
        auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(c.player));

        if (!controller)
            break;

        controller->mLeftStick = {(f32)c.stickL.x / 32767.f, (f32)c.stickL.y / 32767.f};
        controller->mRightStick = {(f32)c.stickR.x / 32767.f, (f32)c.stickR.y / 32767.f};

        u64 buttons = 0;
        memcpy(&buttons, c.buttons, sizeof(c.buttons));
        buttons = convertButtonsSTASToSead(buttons);

        controller->mPadTrig = buttons & ~mPrevButtons[c.player];
        controller->mPadRelease = buttons & mPrevButtons[c.player];
        controller->mPadHold = buttons;
        mPrevButtons[c.player] = buttons;

        if (controller->mPadTrig.isOnBit(sead::Controller::cPadIdx_1 /*Left Stick*/))
            mIsUseAbsJoystick = !mIsUseAbsJoystick;

        break;
    }

    case CommandType::MOTION: {
        CmdMotion c = *(CmdMotion*)cmd->data;

        if (c.player > 1)
            break;

        sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
        auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(c.player));

        if (!controller)
            break;

        controller->mSixAxisSensorCount = 2;

        auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 0);
        auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 1);
        auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 0);
        auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 1);

        switch (c.conId) {
        case 0:
        case 2:
            accelLeft->mAcceleration = c.accel;
            gyroLeft->mAngularVel = c.gyro;
            gyroLeft->mDirection = sead::Matrix33f::ident;

            if (c.conId != 2)
                break;
            [[fallthrough]];
        case 1:
            accelRight->mAcceleration = c.accel;
            gyroRight->mAngularVel = c.gyro;
            gyroRight->mDirection = sead::Matrix33f::ident;

            break;
        }
        break;
    }

    case CommandType::SAVEFILE: {
        CmdSaveFile* c = (CmdSaveFile*)cmd->data;
        if (StageScene* scene = tryGetStageScene(); scene) {
            if (GameDataHolder* holder = tryGetGameDataHolder(); holder) {
                scene->mStagePauseMenu->mStateOption->mIsLoadData = true;
                holder->requestSetPlayingFileId(c->reload ? holder->getPlayingFileId() : c->fileId);
                scene->kill();
            }
        }
        break;
    }

    case CommandType::GO: {
        CmdGo c;
        u8* data = cmd->data;
        u64 cursor = 0;
        c.scenario = read<s8>(data, &cursor);
        c.subScenario = read<s8>(data, &cursor);
        c.returnPrev = read<bool>(data, &cursor);
        cursor++;
        c.stageLen = read<u16>(data, &cursor);
        c.stageName = (char*)&data[cursor];
        cursor += c.stageLen;
        cursor = hk::alignUp(cursor, 4);
        c.entrLen = read<u16>(data, &cursor);
        c.entrId = (char*)&data[cursor];

        GameDataHolder* holder = tryGetGameDataHolder();
        ChangeStageInfo info(holder, c.entrId, c.stageName, c.returnPrev, c.scenario, (ChangeStageInfo::SubScenarioType)c.subScenario);
        holder->changeNextStage(&info);

        break;
    }

    case CommandType::TPMARIO: {
        CmdTpMarCap c = *(CmdTpMarCap*)cmd->data;
        PlayerActorHakoniwa* p = tryGetPlayerActorHakoniwa();
        if (!p)
            break;
        p->startDemoPuppetable();
        al::setTrans(p, c.pos);
        if (c.rot != sead::Quatf{0, 0, 0, 0})
            al::updatePoseQuat(p, c.rot);
        p->endDemoPuppetable();
        break;
    }

    case CommandType::TPCAP: {
        CmdTpMarCap c = *(CmdTpMarCap*)cmd->data;
        PlayerActorHakoniwa* p = tryGetPlayerActorHakoniwa();
        if (!p)
            break;
        HackCap* cap = p->mHackCap;
        if (!cap)
            break;
        cap->startPuppet();
        al::setTrans(cap, c.pos);
        if (c.rot != sead::Quatf{0, 0, 0, 0})
            al::updatePoseQuat(cap, c.rot);
        cap->endPuppet();
        break;
    }

    case CommandType::ABSSTICK: {
        CmdAbsStick* c = (CmdAbsStick*)cmd->data;
        mIsUseAbsJoystick = c->enable;
        break;
    }

    case CommandType::SPEEDUP: {
        CmdSpeedup* c = (CmdSpeedup*)cmd->data;
        mSpeed = c->speed;
        break;
    }

    case CommandType::PAUSE: {
        WindowStagePause* win = (WindowStagePause*)DevGuiManager::instance()->getWindow(windowNameStagePause);
        win->requestPause();
        break;
    }

    case CommandType::DEMO: {
        CmdDemo* c = (CmdDemo*)cmd->data;
        PlayerActorHakoniwa* p = tryGetPlayerActorHakoniwa();
        if (p) {
            if (c->enable)
                p->startDemoPuppetable();
            else
                p->endDemoPuppetable();
        }
        break;
    }

    case CommandType::COMMENT:
    case CommandType::INVALID:
        break;

    default: {
        Logger::log("Command 0x%04X unknown or not implemented\n", cmd->type);
        break;
    }
    }
    delete cmd;
}

void STAS::exeUpdate() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());

    sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
    for (int player = 0; player < 2; player++) {
        auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(player));
        if (controller) {
            mPrevButtons[player] = controller->mPadHold;
            controller->mPadTrig.makeAllZero();
            controller->mPadRelease.makeAllZero();
        }
    }

    while (mCurFrame <= mScript->getFrames() && mCurFrame >= mNextFrame) {
        hk::ValueOrResult<Command*> cmd = mScript->tryReadCommand();
        if (!cmd.hasValue())
            break;
        applyCommand(cmd.getInnerValue());
    }

    if (mCurFrame >= mScript->getFrames()) {
        auto* ghostMgr = GhostManager::instance();
        if (ghostMgr->isRecording())
            ghostMgr->setNerveRecordEnd();
        endScript();
        return;
    }
    mCurFrame++;
}

bool STAS::isRunning() {
    return al::isNerve(this, &NrvSTAS.Update);
}

#define hookCode(ORIG)                                                                                                                               \
    STAS* tas = STAS::instance();                                                                                                                    \
    if (tas && tas->isRunning())                                                                                                                     \
        return;                                                                                                                                      \
    orig(ORIG);

HkTrampoline inputHookCont = [](TrampolineStatic(), al::NpadController* controller) -> void { hookCode(controller); };
HkTrampoline inputHookAccel = [](TrampolineStatic(), al::JoyPadAccelerometerAddon* addon) -> void { hookCode(addon); };
HkTrampoline inputHookGyro = [](TrampolineStatic(), al::PadGyroAddon* addon) -> void { hookCode(addon); };

void STAS::installHooks() {
    inputHookCont.installAtSym<"_ZN2al14NpadController9calcImpl_Ev">();
    inputHookAccel.installAtSym<"_ZN2al24JoyPadAccelerometerAddon4calcEv">();
    inputHookGyro.installAtSym<"_ZN2al12PadGyroAddon4calcEv">();
}

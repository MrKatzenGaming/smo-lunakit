#include "smo-tas/STAS.h"

#include "hk/Result.h"
#include "hk/ValueOrResult.h"
#include "hk/hook/Trampoline.h"
#include "hk/prim/traits/Integer.h"

#include "nn/fs/fs_directories.h"

#include "sead/controller/seadControllerMgr.h"
#include "sead/heap/seadHeapMgr.h"

#include "Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Base/StringUtil.h"
#include "al/Library/Controller/InputFunction.h"
#include "al/Library/Nerve/NerveSetupUtil.h"
#include "al/Library/Nerve/NerveUtil.h"

#include "game/Player/HackCap.h"
#include "game/System/GameSystem.h"
#include "game/Util/StageInputFunction.h"

#include "custom/al/Pad/JoyPadAccelerometerAddon.h"
#include "custom/al/Pad/NpadController.h"
#include "custom/al/Pad/PadGyroAddon.h"

#include <cstring>

#include "MapObj/ChangeStageInfo.h"
#include "Player/PlayerActorHakoniwa.h"
#include "System/GameDataHolder.h"
#include "devgui/DevGuiManager.h"
#include "ghost/GhostManager.h"
#include "heap/seadHeapMgr.h"
#include "helpers/GetHelper.h"
#include "helpers/fsHelper.h"
#include "logger/Logger.hpp"

hk::Result Script::load(const char* path) {
    sead::ScopedCurrentHeapSetter s(mHeap);
    FsHelper::LoadData data{.path = path};
    if (FsHelper::loadFileFromPath(data).failed())
        return hk::ResultFailed();

    mData = (u8*)data.buffer;
    mCursor = 0;
    mFileSize = data.bufSize;

    mFileHeader = *(FileHeader*)&mData[mCursor];
    mCursor += sizeof(mFileHeader);

    if (mFileHeader.magic[0] != 'S' || mFileHeader.magic[1] != 'T' || mFileHeader.magic[2] != 'A' || mFileHeader.magic[3] != 'S') {
        unload();
        Logger::log("Wrong Header\n");
        return hk::ResultFailed();
    }

    if (mFileHeader.title_id != 0x0100000000010000) {
        unload();
        Logger::log("Wrong Title ID\n");
        return hk::ResultFailed();
    }

    switch (mFileHeader.version) {
    case 1: {
        mScriptHeader.cmdCount = read<u32>();
        mScriptHeader.frameCount = read<u32>();
        mScriptHeader.editingSeconds = read<u32>();

        for (int i = 0; i < 8; i++)
            mScriptHeader.conTypes[i] = read<u8>();

        mCursor += read<u16>();
        mCursor += read<u32>();
        mCursor += read<u32>();

        mCursor = hk::alignUp(mCursor, 4);
        return hk::ResultSuccess();
    }
    default:
        Logger::log("Unsupported STAS version\n");
        unload();
        return hk::ResultNotImplemented();
    }
}

void Script::unload() {
    free(mData);
    mCursor = 0;
    mFileHeader = FileHeader();
    mScriptHeader = ScriptHeader();
}

hk::ValueOrResult<Command*> Script::tryReadCommand() {
    sead::ScopedCurrentHeapSetter s(DevGuiManager::instance()->getHeap());

    // Logger::log("Cursor: %d, Data: %d\n", mCursor, mData ? 1 : 0);

    if (mCursor >= mFileSize)
        return hk::ResultOutOfRange();

    CommandType type = read<CommandType>();
    // Logger::log("Type: %d\n", type);

    u64 size = read<u64>() & 0xffffffffffff;
    mCursor -= 2;
    // Logger::log("Size: %d\n", size);
    if (size == 0)
        return hk::ResultOutOfRange();

    Command* c = new Command{.type = type, .size = size, .data = &mData[mCursor]};
    mCursor += size;

    return hk::ValueOrResult(c);
}

namespace {
NERVE_IMPL(STAS, Update);
NERVE_IMPL(STAS, Wait);
NERVE_IMPL(STAS, Record);

NERVES_MAKE_STRUCT(STAS, Update, Wait, Record)

}  // namespace

SEAD_SINGLETON_DISPOSER_IMPL(STAS);

HkTrampoline inputHookCont = [](TrampolineStatic(), al::NpadController* controller) -> void {
    auto* tas = STAS::instance();
    if (!tas || !tas->isRunning())
        orig(controller);
};
HkTrampoline inputHookAccel = [](TrampolineStatic(), al::JoyPadAccelerometerAddon* addon) -> void {
    auto* tas = STAS::instance();
    if (!tas || !tas->isRunning())
        orig(addon);
};
HkTrampoline inputHookGyro = [](TrampolineStatic(), al::PadGyroAddon* addon) -> void {
    auto* tas = STAS::instance();
    if (!tas || !tas->isRunning())
        orig(addon);
};

STAS::STAS() : al::NerveExecutor("STAS") {
    initNerve(&NrvSTAS.Wait, 0);
    if (!FsHelper::isDirExist("sd:/smo/tas"))
        nn::fs::CreateDirectory("sd:/smo/tas");
    if (!FsHelper::isDirExist(TAS_SCRIPTPATH))
        nn::fs::CreateDirectory(TAS_SCRIPTPATH);

    updateDir();
}

void STAS::installHooks() {
    inputHookCont.installAtSym<"_ZN2al14NpadController9calcImpl_Ev">();
    inputHookAccel.installAtSym<"_ZN2al24JoyPadAccelerometerAddon4calcEv">();
    inputHookGyro.installAtSym<"_ZN2al12PadGyroAddon4calcEv">();
}

STAS::~STAS() = default;

void STAS::updateDir() {
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

bool STAS::tryStartScript() {
    if (tryLoadScript()) {
        startScript();
        return true;
    }
    return false;
}

bool STAS::tryLoadScript() {
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

    // if (!(al::isEndWithString(mLoadedEntry.mName, ".stas") || al::isEndWithString(mLoadedEntry.mName, ".STAS")))
    //     return false;

    mScript = new Script(DevGuiManager::instance()->getHeap());
    if (mScript->load(scriptPath.cstr()).failed()) {
        endScript();
        return false;
    };

    if (mScript->getPlayerCount() > 2) {
        Logger::log("Player Count > 2\n");
        endScript();
        return false;
    }

    return true;
}

void STAS::startScript() {
    bool isWait = false;

    // check if script uses 2-player mode
    if (mScript->is2P() != rs::isSeparatePlay(mScene)) {
        al::GamePadSystem* gamePadSystem = GameSystemFunction::getGameSystem()->mGamePadSystem;
        if (mScript->is2P()) {
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

    al::setNerve(this, &NrvSTAS.Update);
}

void STAS::endScript() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());
    al::setNerve(this, &NrvSTAS.Wait);
    mFrameIndex = -1;
    mNextFrame = -1;
    mPrevButtons[0] = 0;
    mPrevButtons[1] = 0;
    delete[] mScript;
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
            else if (i == cSTAS_LeftStickLeft)
                mask.setBit(sead::Controller::cPadIdx_LeftStickLeft);
            else if (i == cSTAS_LeftStickUp)
                mask.setBit(sead::Controller::cPadIdx_LeftStickUp);
            else if (i == cSTAS_LeftStickRight)
                mask.setBit(sead::Controller::cPadIdx_LeftStickRight);
            else if (i == cSTAS_LeftStickDown)
                mask.setBit(sead::Controller::cPadIdx_LeftStickDown);
            else if (i == cSTAS_RightStickLeft)
                mask.setBit(sead::Controller::cPadIdx_RightStickLeft);
            else if (i == cSTAS_RightStickUp)
                mask.setBit(sead::Controller::cPadIdx_RightStickUp);
            else if (i == cSTAS_RightStickRight)
                mask.setBit(sead::Controller::cPadIdx_RightStickRight);
            else if (i == cSTAS_RightStickDown)
                mask.setBit(sead::Controller::cPadIdx_RightStickDown);
        }
    }

    return mask;
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

        sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
        auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(c.player));

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
            setIsUseAbsoluteJoystick(!isUseAbsoluteJoystick());

        break;
    }
    case CommandType::MOTION: {
        CmdMotion c = *(CmdMotion*)cmd->data;

        sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
        auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(c.player));

        controller->mPadAccelerationDeviceNum = 2;

        auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 0);
        auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Accelerometer, 1);
        auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 0);
        auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAddon_Gyro, 1);

        switch (c.conId) {
        case 0:
        case 2:
            accelLeft->mAcceleration = c.accel;
            gyroLeft->mAngularVel = c.gyro;
            if (c.conId != 2)
                break;
        case 1:
            accelRight->mAcceleration = c.accel;
            gyroRight->mAngularVel = c.gyro;
            break;
        }
        break;
    }

    case CommandType::GO: {
        CmdGo c;
        u8* cursor = cmd->data;
        c.scenario = cursor[0];
        c.subScenario = cursor[1];
        c.returnPrev = cursor[2];
        c.stageLen = cursor[4];
        c.stageName = (char*)&cursor[6];
        c.entrLen = cursor[4 + 2 + c.stageLen];
        c.entrId = (char*)&cursor[6 + c.stageLen + 2];

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
        al::updatePoseQuat(cap, c.rot);
        cap->endPuppet();
        break;
    }

    default:
        Logger::log("Command %d unknown or not implemented\n", cmd->type);
        break;
    }
    delete cmd;
}

void STAS::exeUpdate() {
    sead::ScopedCurrentHeapSetter heapSetter(DevGuiManager::instance()->getHeap());

    while (mFrameIndex <= mScript->getFrames() && mFrameIndex >= mNextFrame) {
        hk::ValueOrResult<Command*> cmdR = mScript->tryReadCommand();
        if (!cmdR.hasValue())
            break;

        Command* cmd = cmdR.getInnerValue();
        applyCommand(cmd);
    }

    if (mFrameIndex >= mScript->getFrames()) {
        auto* ghostMgr = GhostManager::instance();
        if (ghostMgr->isRecording())
            ghostMgr->setNerveRecordEnd();
        // Logger::log("Ended Script on Step: %d\n", al::getNerveStep(this));
        // al::setNerve(this, &NrvTASWait);
        endScript();
        return;
    }
    mFrameIndex++;
}

void STAS::exeWait() {}

void STAS::exeRecord() {}

bool STAS::isRunning() {
    return al::isNerve(this, &NrvSTAS.Update);
}

#include "devgui/settings/SettingsHooks.h"

#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/hook/a64/Assembler.h"
#include "hk/ro/RoUtil.h"

#include "al/Library/Bgm/BgmLineFunction.h"
#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/LiveActor/ActorFlagFunction.h"
#include "al/Library/LiveActor/ActorMovementFunction.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Math/MathUtil.h"

#include "game/Layout/StageSceneLayout.h"
#include "game/Player/HackCap.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerWallActionHistory.h"

#include "custom/game/GameData/PlayerHitPointData.h"
#include "custom/game/Layout/MapMini.h"
#include "custom/game/Player/HackCap/PlayerCapActionHistory.h"
#include "custom/game/Scene/StageScene.h"

#include <cstring>

#include "System/GameDataHolder.h"
#include "System/GameDataHolderWriter.h"
#include "devgui/DevGuiManager.h"
#include "devgui/settings/DevGuiSettings.h"
#include "devgui/windows/MoonRefresh/WindowMoonRefresh.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"

HkTrampoline<void, StageScene*> ControlHook = hk::hook::trampoline([](StageScene* scene) -> void {
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa(scene);
    DevGuiSettings* set = DevGuiManager::instance()->getSettings();

    if (player && set->getStateByName("Infinite Cap Bounce")) {
        player->mCapActionHistory->clearCapJump();
        player->mCapActionHistory->clearWallAirLimit();
    }

    if (player && set->getStateByName("Clear Wall Jump Limits"))
        player->mWallActionHistory->reset();

    if (!set->getStateByName("Display HUD") && scene->mStageSceneLayout->isWait()) {
        scene->mStageSceneLayout->end();
        MapMini* compass = scene->mStageSceneLayout->mMapMini;
        if (compass->mIsAlive)
            compass->end();
    }

    if (!set->getStateByName("Play Music")) {
        if (al::isPlayingBgm(scene))
            al::stopAllBgm(scene, 0);
    }

    ControlHook.orig(scene);
});

HkTrampoline<void, PlayerActorHakoniwa*> NoclipMovementHook = hk::hook::trampoline([](PlayerActorHakoniwa* player) -> void {
    static bool wasNoclipOn = false;
    bool isNoclip = DevGuiManager::instance()->getSettings()->getStateByName("Noclip");

    if (!isNoclip && wasNoclipOn) {
        al::onCollide(player);
        player->endDemoPuppetable();
    }
    wasNoclipOn = isNoclip;

    if (!isNoclip) {
        NoclipMovementHook.orig(player);
        return;
    }

    if (isNoclip) {
        static float speed = 20.0f;
        static float speedMax = 250.0f;
        static float vspeed = 10.0f;
        static float speedGain = 0.0f;

        sead::Vector3f* playerPos = al::getTransPtr(player);
        const sead::Vector3f* cameraPos = &al::getCameraPos(player, 0);
        const sead::Vector2 leftStick = {InputHelper::getLeftStickX(), InputHelper::getLeftStickY()};

        player->startDemoPuppetable();
        al::offCollide(player);
        al::setVelocityZero(player);
        player->exeJump();

        float d = sqrt(al::powerIn(playerPos->x - cameraPos->x, 2) + (al::powerIn(playerPos->z - cameraPos->z, 2)));
        float vx = ((speed + speedGain) / d) * (playerPos->x - cameraPos->x);
        float vz = ((speed + speedGain) / d) * (playerPos->z - cameraPos->z);

        playerPos->x -= leftStick.x * vz;
        playerPos->z += leftStick.x * vx;

        playerPos->x += leftStick.y * vx;
        playerPos->z += leftStick.y * vz;

        if (InputHelper::isHoldX() || InputHelper::isHoldY())
            speedGain += 0.5f;
        if (InputHelper::isHoldA() || InputHelper::isHoldB())
            speedGain -= 0.5f;
        if (speedGain <= 0.0f)
            speedGain = 0.0f;
        if (speedGain >= speedMax)
            speedGain = speedMax;

        if (InputHelper::isHoldZL())
            playerPos->y -= (vspeed + speedGain / 3);
        if (InputHelper::isHoldZR())
            playerPos->y += (vspeed + speedGain / 3);
    }

    NoclipMovementHook.orig(player);
});

HkTrampoline<bool, StageScene*> SaveHook = hk::hook::trampoline([](StageScene* scene) -> bool {
    if (DevGuiManager::instance()->getSettings()->getStateByName("Autosave"))
        return SaveHook.orig(scene);

    return false;
});

HkTrampoline<bool, void*> CheckpointWarpHook = hk::hook::trampoline([](void* thisPtr) -> bool {
    if (DevGuiManager::instance()->getSettings()->getStateByName("Always Allow Checkpoints"))
        return true;
    return CheckpointWarpHook.orig(thisPtr);
});

class ShineInfo;

HkTrampoline<bool, GameDataHolderWriter, const ShineInfo*> GreyShineRefreshHook =
    hk::hook::trampoline([](GameDataHolderWriter writer, const ShineInfo* shineInfo) -> bool {
        if (WindowMoonRefresh::getIsGrayRefreshEnabled())
            return false;
        else
            return GreyShineRefreshHook.orig(writer, shineInfo);
    });

HkTrampoline<void, GameDataHolderWriter, const ShineInfo*> ShineRefreshHook =
    hk::hook::trampoline([](GameDataHolderWriter writer, const ShineInfo* shineInfo) -> void {
        ptr addr = hk::sail::lookupSymbolFromDb<>("$MoonRefreshText");
        ptr offset = addr - hk::ro::getMainModule()->range().start();
        const char* text = WindowMoonRefresh::getRefreshText();
        hk::ro::getMainModule()->writeRo(offset, text, strlen(text) + 1);

        if (!WindowMoonRefresh::getIsRefreshEnabled())
            ShineRefreshHook.orig(writer, shineInfo);
    });

HkTrampoline<int, GameDataHolder*, bool*, int> DisableMoonLockHook =
    hk::hook::trampoline([](GameDataHolder* thisPtr, bool* isCrashList, int worldID) -> int {
        int lockSize = DisableMoonLockHook.orig(thisPtr, isCrashList, worldID);

        if (DevGuiManager::instance()->getSettings()->getStateByName("Disable Kingdom Moon Lock"))
            return 0;

        return lockSize;
    });

HkTrampoline<bool, void*> ButtonMotionRollHook = hk::hook::trampoline([](void* thisPtr) -> bool {
    if (DevGuiManager::instance()->getSettings()->getStateByName("Button Motion Roll"))
        return true;

    return ButtonMotionRollHook.orig(thisPtr);
});

HkTrampoline<void, PlayerHitPointData*> NoDamageHook = hk::hook::trampoline([](PlayerHitPointData* hitPointData) -> void {
    if (!DevGuiManager::instance()->getSettings()->getStateByName("No Damage"))
        return NoDamageHook.orig(hitPointData);
});

void LoadCurrentFilePatch() {
    __asm("LDR W20, [X8, #0x3C]");

    GameDataHolder* holder = tryGetGameDataHolder();

    s64 fileId;

    if (DevGuiManager::instance()->getSettings()->getStateByName("Allow Loading Current File")) {
        __asm("MOV X0, 0xFF");
    } else {
        fileId = holder->getPlayingFileId();
        __asm("MOV X0, %[input]" : [input] "=r"(fileId));
    }
}

void exlSetupSettingsHooks() {
    ControlHook.installAtSym<"_ZN10StageScene7controlEv">();
    NoclipMovementHook.installAtSym<"_ZN19PlayerActorHakoniwa8movementEv">();
    SaveHook.installAtSym<"_ZNK10StageScene12isEnableSaveEv">();
    CheckpointWarpHook.installAtSym<"_ZNK9MapLayout22isEnableCheckpointWarpEv">();
    GreyShineRefreshHook.installAtSym<"_ZN16GameDataFunction10isGotShineE22GameDataHolderAccessorPK9ShineInfo">();
    ShineRefreshHook.installAtSym<"_ZN16GameDataFunction11setGotShineE20GameDataHolderWriterPK9ShineInfo">();
    DisableMoonLockHook.installAtSym<"_ZNK14GameDataHolder18findUnlockShineNumEPbi">();
    ButtonMotionRollHook.installAtSym<"_ZNK23PlayerJudgeStartRolling21isTriggerRestartSwingEv">();
    NoDamageHook.installAtSym<"_ZN16GameDataFunction12damagePlayerE20GameDataHolderWriter">();
    exlSetupDemoHooks();

    hk::hook::writeBranchLinkAtMainOffset(0x004e7f84, LoadCurrentFilePatch);
    hk::hook::a64::assemble<"nop">().installAtMainOffset(0x004e7f84 + 0x4);
}

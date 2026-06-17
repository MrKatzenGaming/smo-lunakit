#include "devgui/settings/SettingsHooks.h"

#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"

#include "al/Library/Bgm/BgmLineFunction.h"
#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Math/MathUtil.h"

#include "game/Layout/StageSceneLayout.h"
#include "game/Player/HackCap.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerCapActionHistory.h"
#include "game/Player/PlayerWallActionHistory.h"
#include "game/System/GameDataHolder.h"
#include "game/System/GameDataHolderWriter.h"
#include "game/System/PlayerHitPointData.h"

#include "custom/game/Layout/MapMini.h"
#include "custom/game/Scene/StageScene.h"

#include <cstring>

#include "System/GameDataFile.h"
#include "devgui/DevGuiManager.h"
#include "devgui/settings/DevGuiSettings.h"
#include "devgui/windows/MoonRefresh/WindowMoonRefresh.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"

HkTrampoline ControlHook = [](TrampolineStatic(), StageScene* scene) -> void {
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

    orig(scene);
};

HkTrampoline NoclipMovementHook = [](TrampolineStatic(), PlayerActorHakoniwa* player) -> void {
    static bool wasNoclipOn = false;
    bool isNoclip = DevGuiManager::instance()->getSettings()->getStateByName("Noclip");

    if (!isNoclip && wasNoclipOn)
        player->endDemoPuppetable();
    wasNoclipOn = isNoclip;

    if (!isNoclip) {
        orig(player);
        return;
    }

    if (isNoclip) {
        static float speed = 20.0f;
        static float speedMax = 250.0f;
        static float vspeed = 10.0f;
        static float speedGain = 0.0f;

        sead::Vector3f* playerPos = al::getTransPtr(player);
        const sead::Vector3f* cameraPos = &al::getCameraPos(player, 0);
        const sead::Vector2 leftStick = InputHelper::getLeftStick();

        player->startDemoPuppetable();

        float d = sqrt(al::powerIn(playerPos->x - cameraPos->x, 2) + (al::powerIn(playerPos->z - cameraPos->z, 2)));
        float vx = ((speed + speedGain) / d) * (playerPos->x - cameraPos->x);
        float vz = ((speed + speedGain) / d) * (playerPos->z - cameraPos->z);

        playerPos->x -= leftStick.x * vz;
        playerPos->z += leftStick.x * vx;

        playerPos->x += leftStick.y * vx;
        playerPos->z += leftStick.y * vz;

        if (InputHelper::isHoldX() || InputHelper::isHoldY())
            speedGain = al::clamp(speedGain + 0.5f, 0.f, speedMax);

        if (InputHelper::isHoldA() || InputHelper::isHoldB())
            speedGain = al::clamp(speedGain - 0.5f, 0.f, speedMax);

        if (InputHelper::isHoldZL())
            playerPos->y -= (vspeed + speedGain / 3);
        if (InputHelper::isHoldZR())
            playerPos->y += (vspeed + speedGain / 3);
    }

    orig(player);
};

HkTrampoline SaveHook = [](TrampolineStatic(), StageScene* scene) -> bool {
    if (DevGuiManager::instance()->getSettings()->getStateByName("Autosave"))
        return orig(scene);

    return false;
};

HkTrampoline CheckpointWarpHook = [](TrampolineStatic(), void* thisPtr) -> bool {
    if (DevGuiManager::instance()->getSettings()->getStateByName("Always Allow Checkpoints"))
        return true;
    return orig(thisPtr);
};

HkTrampoline GreyShineRefreshHook = [](TrampolineStatic(), GameDataHolderWriter writer, const void* shineInfo) -> bool {
    if (WindowMoonRefresh::getIsGrayRefreshEnabled())
        return false;
    else
        return orig(writer, shineInfo);
};

HkTrampoline ShineRefreshHook = [](TrampolineStatic(), GameDataHolderWriter writer, const void* shineInfo) -> void {
    ptr addr = hk::sail::lookupSymbolFromDb<>("$MoonRefreshText");
    ptr offset = addr - hk::ro::getMainModule()->range().start();
    const char* text = WindowMoonRefresh::getRefreshText();
    hk::ro::getMainModule()->writeRo(offset, text, strlen(text) + 1);

    if (!WindowMoonRefresh::getIsRefreshEnabled())
        orig(writer, shineInfo);
};

HkTrampoline DisableMoonLockHook = [](TrampolineStatic(), GameDataHolder* thisPtr, bool* isCrashList, int worldID) -> int {
    int lockSize = orig(thisPtr, isCrashList, worldID);

    if (DevGuiManager::instance()->getSettings()->getStateByName("Disable Kingdom Moon Lock"))
        return 0;

    return lockSize;
};

HkTrampoline ButtonMotionRollHook = [](TrampolineStatic(), void* thisPtr) -> bool {
    if (DevGuiManager::instance()->getSettings()->getStateByName("Button Motion Roll"))
        return true;

    return orig(thisPtr);
};

HkTrampoline NoDamageHook = [](TrampolineStatic(), PlayerHitPointData* hitPointData) -> void {
    if (!DevGuiManager::instance()->getSettings()->getStateByName("No Damage"))
        return orig(hitPointData);
};

int LoadCurrentFilePatch(GameDataHolder* holder) {
    return DevGuiManager::instance()->getSettings()->getStateByName("Allow Loading Current File") ? 5 : holder->getPlayingFileId();
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

    hk::hook::writeBranchLinkAtSym<"$loadCurrentFileHook">(LoadCurrentFilePatch);
}

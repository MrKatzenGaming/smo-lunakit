#include "SettingsHooks.h"
#include <game/Layout/MapMini.h>
#include <game/Scene/StageSceneLayout.h>

#include "al/util/NerveUtil.h"
#include "Library/LiveActor/ActorPoseKeeper.h"

#include "devgui/DevGuiManager.h"
#include "devgui/settings/DevGuiSettings.h"
#include "game/Sequence/HakoniwaSequence.h"
#include "game/Player/PlayerFunction.h"
#include "game/Player/HackCap.h"

#include "patch/code_patcher.hpp"

#include "helpers/GetHelper.h"
#include "helpers/NrvFind/NrvFindHelper.h"
#include "helpers/NrvFind/player/NrvPlayerActorHakoniwa.h"

#include "logger/Logger.hpp"

void exlSetupDemoHooks();

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void exlSetupDemoHooks();

HOOK_DEFINE_TRAMPOLINE(ControlHook) {
    static void Callback(StageScene *scene) {
        PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa(scene);
        DevGuiSettings* set = DevGuiManager::instance()->getSettings();

        if(player && set->getStateByName("Infinite Cap Bounce")) {
            player->mHackCap->mCapActionHistory->clearCapJump();
            player->mHackCap->mCapActionHistory->clearWallAirLimit();
        }

        if(player && set->getStateByName("Clear Wall Jump Limits"))
            player->mWallActionHistory->reset();

        if(!set->getStateByName("Display HUD") && scene->mStageSceneLayout->isWait()) {
            scene->mStageSceneLayout->end();
            MapMini* compass = scene->mStageSceneLayout->mMapMiniLyt;
            if (compass->mIsAlive) compass->end();
        }

        if(!set->getStateByName("Play Music")) {
            if (al::isPlayingBgm(scene)) {
                al::stopAllBgm(scene, 0);
            }
        }

        Orig(scene);
    }
};

HOOK_DEFINE_TRAMPOLINE(NoclipMovementHook) {
    static void Callback(PlayerActorHakoniwa *player) {
        static bool wasNoclipOn = false;
        bool isNoclip = DevGuiManager::instance()->getSettings()->getStateByName("Noclip");

        if (!isNoclip && wasNoclipOn)
            al::onCollide(player);
        wasNoclipOn = isNoclip;

        if(!isNoclip) {
            Orig(player);
            return;
        }

        if (isNoclip) {
            static float speed = 20.0f;
            static float speedMax = 250.0f;
            static float vspeed = 10.0f;
            static float speedGain = 0.0f;

            sead::Vector3f *playerPos = al::getTransPtr(player);
            sead::Vector3f *cameraPos = al::getCameraPos(player, 0);
            sead::Vector2f *leftStick = al::getLeftStick(-1);

            const al::Nerve* hipDropNrv = NrvFindHelper::getNerveAt(nrvPlayerActorHakoniwaHipDrop);
            if(al::isNerve(player, hipDropNrv))
                NrvFindHelper::setNerveAt(player, nrvPlayerActorHakoniwaWait);

            player->exeJump();
            al::offCollide(player);
            al::setVelocityZero(player);

            // Mario slightly goes down even when velocity is 0. This is a hacky fix for that.
            playerPos->y += 1.5f;

            float d = sqrt(al::powerIn(playerPos->x - cameraPos->x, 2) + (al::powerIn(playerPos->z - cameraPos->z, 2)));
            float vx = ((speed + speedGain) / d) * (playerPos->x - cameraPos->x);
            float vz = ((speed + speedGain) / d) * (playerPos->z - cameraPos->z);

            playerPos->x -= leftStick->x * vz;
            playerPos->z += leftStick->x * vx;

            playerPos->x += leftStick->y * vx;
            playerPos->z += leftStick->y * vz;

            if (al::isPadHoldX(-1) || al::isPadHoldY(-1)) speedGain += 0.5f;
            if (al::isPadHoldA(-1) || al::isPadHoldB(-1)) speedGain -= 0.5f;
            if (speedGain <= 0.0f) speedGain = 0.0f;
            if (speedGain >= speedMax) speedGain = speedMax;

            if (al::isPadHoldZL(-1)) playerPos->y -= (vspeed + speedGain / 3);
            if (al::isPadHoldZR(-1)) playerPos->y += (vspeed + speedGain / 3);
        }

        Orig(player);
    }
};

HOOK_DEFINE_TRAMPOLINE(SaveHook) {
    static bool Callback(StageScene* scene) {
        if (DevGuiManager::instance()->getSettings()->getStateByName("Autosave"))
            return Orig(scene);

        return false;
    }
};

HOOK_DEFINE_TRAMPOLINE(CheckpointWarpHook) {
    static bool Callback(void* thisPtr) {
        if (DevGuiManager::instance()->getSettings()->getStateByName("Always Allow Checkpoints"))
            return true;

        return Orig(thisPtr);
    }
};

class ShineInfo;

HOOK_DEFINE_TRAMPOLINE(GreyShineRefreshHook) {
    static bool Callback(GameDataHolderWriter writer, ShineInfo const* shineInfo) {
         if( WindowMoonRefresh::getIsGrayRefreshEnabled()) return false;
         else return Orig(writer, shineInfo);
    }
};

HOOK_DEFINE_TRAMPOLINE(ShineRefreshHook) {
    static void Callback(GameDataHolderWriter writer, ShineInfo const* shineInfo) {

        exl::util::RwPages ShineRefreshText(exl::util::modules::GetTargetOffset(0x01832301), 24);
        
        strncpy((char*)ShineRefreshText.GetRw(),WindowMoonRefresh::getRefreshText()  , 24);
    
        if (!WindowMoonRefresh::getIsRefreshEnabled()) 
            Orig(writer, shineInfo);
    }
};

HOOK_DEFINE_TRAMPOLINE(DisableMoonLockHook) {
    static int Callback(GameDataHolder *thisPtr, bool *isCrashList, int worldID) {
        int lockSize = Orig(thisPtr, isCrashList, worldID);

        if (DevGuiManager::instance()->getSettings()->getStateByName("Disable Kingdom Moon Lock"))
            return 0;
        
        return lockSize;
    }
};

HOOK_DEFINE_TRAMPOLINE(ButtonMotionRollHook) {
    static bool Callback(void* thisPtr) {
        if (DevGuiManager::instance()->getSettings()->getStateByName("Button Motion Roll"))
            return true;

        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(NoDamageHook){
    static void Callback(PlayerHitPointData* hitPointData) {
        if (!DevGuiManager::instance()->getSettings()->getStateByName("No Damage"))
            return Orig(hitPointData);
    }
};

void LoadCurrentFilePatch()
{   
    __asm ("LDR W20, [X8, #0x3C]");

    GameDataHolder* holder;
    __asm ("MOV %[result], X0" : [result] "=r" (holder));

    s32 fileId = DevGuiManager::instance()->getSettings()->getStateByName("Allow Loading Current File") ? holder->getPlayingFileId() -1 : 5;

    __asm ("MOV X0, %[input]" : [input] "=r" (fileId));
}

void exlSetupSettingsHooks()
{
    ControlHook::InstallAtSymbol("_ZN10StageScene7controlEv");
    NoclipMovementHook::InstallAtSymbol("_ZN19PlayerActorHakoniwa8movementEv");
    SaveHook::InstallAtSymbol("_ZNK10StageScene12isEnableSaveEv");
    CheckpointWarpHook::InstallAtSymbol("_ZNK9MapLayout22isEnableCheckpointWarpEv");
    GreyShineRefreshHook::InstallAtSymbol("_ZN16GameDataFunction10isGotShineE22GameDataHolderAccessorPK9ShineInfo");
    DisableMoonLockHook::InstallAtSymbol("_ZNK14GameDataHolder18findUnlockShineNumEPbi");
    ButtonMotionRollHook::InstallAtSymbol("_ZNK23PlayerJudgeStartRolling21isTriggerRestartSwingEv");
    NoDamageHook::InstallAtSymbol("_ZN16GameDataFunction12damagePlayerE20GameDataHolderWriter");
    exlSetupDemoHooks();
    ShineRefreshHook::InstallAtSymbol("_ZN16GameDataFunction11setGotShineE20GameDataHolderWriterPK9ShineInfo");
    ButtonMotionRollHook::InstallAtSymbol("_ZNK23PlayerJudgeStartRolling21isTriggerRestartSwingEv");
    NoDamageHook::InstallAtSymbol("_ZN16GameDataFunction12damagePlayerE20GameDataHolderWriter");
    //LoadCurrentFileHook::InstallAtOffset(0x004e7f84);
    exlSetupDemoHooks();
    patch::CodePatcher p(0x004e7f84);
    p.BranchLinkInst((void*) LoadCurrentFilePatch);
    p.WriteInst(inst::Nop());
}

#include "StagePauseHooks.h"
#include <al/util/NerveUtil.h>
#include <game/Scene/StageScene.h>
#include <lib.hpp>
#include "devgui/windows/StagePause/WindowStagePause.h"
#include "hook/inline.hpp"
#include "hook/trampoline.hpp"
#include "stage-pause/StageSceneStateStagePause.h"
#include "devgui/DevGuiManager.h"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

// clang-format off
HOOK_DEFINE_TRAMPOLINE(StageSceneInitHook) {
    static void Callback(StageScene* thisPtr, al::SceneInitInfo* initInfo) {
        Orig(thisPtr, initInfo);
        thisPtr->mStatePause = new StageSceneStateStagePause("StagePause", thisPtr, thisPtr->mAudioSystemPauseController);
        if (thisPtr->mStatePause) {
            al::initNerveState(thisPtr, thisPtr->mStatePause, &StageSceneNrvStagePause::sInstance, "StagePause");
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(ShadowUpdateHook) {
    static void Callback(al::ShadowKeeper* thisPtr) {
        WindowStagePause* win = (WindowStagePause*) DevGuiManager::instance()->getWindow("Stage Pauser");
        if(win->getStagePaused())
            return;

    Orig(thisPtr);
    }
};

// clang-format on
void exlSetupStageSceneHooks() {
    StageSceneInitHook::InstallAtSymbol("_ZN10StageScene4initERKN2al13SceneInitInfoE");
    ShadowUpdateHook::InstallAtSymbol("_ZN2al18ShadowMaskDirector16updateShadowMaskEv");
    exl::patch::CodePatcher p("_ZN16HakoniwaSequence12exeLoadStageEv", 0x1f4);
    p.WriteInst(inst::Movz(reg::X0, sizeof(StageScene)));
    p.Seek("_ZN16HakoniwaSequence7exeMissEv", 0x13c);
    p.WriteInst(inst::Movz(reg::X0, sizeof(StageScene)));
}

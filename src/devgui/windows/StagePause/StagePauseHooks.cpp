#include "devgui/windows/StagePause/StagePauseHooks.h"

#include "hk/hook/Trampoline.h"
#include "hk/hook/a64/Assembler.h"

#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/NerveUtil.h"
#include "Project/Scene/SceneInitInfo.h"

#include "custom/game/Scene/StageScene.h"

#include "devgui/DevGuiManager.h"
#include "devgui/windows/StagePause/WindowStagePause.h"
#include "stage-pause/StageSceneStateStagePause.h"

HkTrampoline StageSceneInitHook = [](TrampolineStatic(), StageScene* thisPtr, al::SceneInitInfo* initInfo) -> void {
    orig(thisPtr, initInfo);
    thisPtr->mStatePause = new StageSceneStateStagePause("StagePause", thisPtr, thisPtr->mAudioSystemPauseController);
    if (thisPtr->mStatePause) {
        al::initNerveState(thisPtr, thisPtr->mStatePause, &StageSceneNrvStagePause::sInstance, "StagePause");
    }
};

HkTrampoline ShadowUpdateHook = [](TrampolineStatic(), al::ShadowKeeper* thisPtr) -> void {
    WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
    if (win && win->getStagePaused())
        return;

    orig(thisPtr);
};

void exlSetupStageSceneHooks() {
    StageSceneInitHook.installAtSym<"_ZN10StageScene4initERKN2al13SceneInitInfoE">();
    ShadowUpdateHook.installAtSym<"_ZN2al18ShadowMaskDirector16updateShadowMaskEv">();

    hk::hook::a64::assemble<"mov x0 #0x4c8">().installAtSym<"$StagePauseHook1">();
    hk::hook::a64::assemble<"mov x0 #0x4c8">().installAtSym<"$StagePauseHook2">();
}

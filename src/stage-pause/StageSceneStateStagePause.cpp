#include "stage-pause/StageSceneStateStagePause.h"

#include "Library/Nerve/NerveUtil.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Nerve/NerveSetupUtil.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Scene/SceneUtil.h"

#include "game/Scene/SceneAudioSystemPauseController.h"

#include "custom/al/effect/EffectSystem.h"
#include "custom/al/graphics/GraphicsFunction.h"

#include "smo-tas/TAS.h"

namespace ScenePlayerFunction {
void startSnapShotMode(const al::Scene*);
void endSnapShotMode(const al::Scene*);
void updatePlayerDither(const al::Scene*);
}  // namespace ScenePlayerFunction

namespace rs {
void updateKitListPostSnapShot(al::Scene*);
}

namespace {
NERVE_IMPL(StageSceneStateStagePause, Wait)

NERVES_MAKE_STRUCT(StageSceneStateStagePause, Wait)

}  // namespace

StageSceneStateStagePause::StageSceneStateStagePause(const char* stateName, al::Scene* parent, SceneAudioSystemPauseController* audioController)
    : NerveStateBase(stateName) {
    initNerve(&NrvStageSceneStateStagePause.Wait, 0);
    mParent = parent;
    mAudioSystemPauseController = audioController;
}

void StageSceneStateStagePause::appear() {
    mIsDead = false;
    al::setNerve(this, &NrvStageSceneStateStagePause.Wait);
}

void StageSceneStateStagePause::kill() {
    mIsDead = true;
}

void StageSceneStateStagePause::exeWait() {
    if (al::isFirstStep(this)) {
        al::pausePadRumble(mParent);
        if (mIsPauseAudio)
            mAudioSystemPauseController->pause(false);
        alGraphicsFunction::setRainUpdateFlag(mParent, false);
        alGraphicsFunction::setFogNoiseUpdateFlag(mParent, false);
    }

    mParent->mLiveActorKit->getEffectSystem()->setIsUpdateKit(true);
    al::updateKitListPrev(mParent);
    // if (*TAS::instance()->isUseAbsoluteJoystick())
    //     al::updateKitList(mParent, "カメラ");            // camera
    al::updateKitList(mParent, "クリッピング");          // clipping
    al::updateKitTable(mParent, "スナップショット");     // snap shot
    al::updateKitList(mParent, "シャドウマスク");        // shadow mask
    al::updateKitList(mParent, "グラフィックス要求者");  // graphics requester
    al::updateKitList(mParent, "２Ｄ（ポーズ無視）");    // 2D (ignore pose)
    ScenePlayerFunction::updatePlayerDither(mParent);
    rs::updateKitListPostSnapShot(mParent);
    mParent->mLiveActorKit->getEffectSystem()->setIsUpdateKit(false);

    if (mIsPaused)
        return;

    al::endPausePadRumble(mParent);
    mAudioSystemPauseController->resume(1);
    alGraphicsFunction::setRainUpdateFlag(mParent, true);
    alGraphicsFunction::setFogNoiseUpdateFlag(mParent, true);

    kill();
}

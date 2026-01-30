#include "custom/game/Scene/StageScene.h"

#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveUtil.h"
#include "al/Library/Scene/SceneUtil.h"

#define NERVE_IMPL_INSTANCE_(Class, Action, ActionFunc)                                                                                              \
    Class##Nrv##Action Class##Nrv##Action::sInstance = {};                                                                                           \
    void Class##Nrv##Action::execute(al::NerveKeeper* keeper) const {                                                                                \
        (keeper->getParent<Class>())->exe##ActionFunc();                                                                                             \
    }

NERVE_IMPL_INSTANCE_(StageScene, StagePause, StagePause);

void StageScene::exeStagePause() {
    al::updateKitListPostOnNerveEnd(this);
    al::updateNerveState(this);
}

#include "helpers/PlayerHelper.h"

#include "al/Library/LiveActor/ActorMovementFunction.h"
#include "al/Library/Player/PlayerUtil.h"

#include "game/Player/PlayerAnimator.h"
#include "game/Sequence/ChangeStageInfo.h"
#include "game/System/GameDataFunction.h"
#include "game/System/GameDataHolderAccessor.h"

namespace PlayerHelper {
void killPlayer(al::LiveActor* actor) {
    PlayerActorHakoniwa* mainPlayer = (PlayerActorHakoniwa*)al::getPlayerActor(actor, 0);

    GameDataFunction::killPlayer(GameDataHolderWriter(actor));
    mainPlayer->startDemoPuppetable();
    al::setVelocityZero(mainPlayer);
    mainPlayer->mAnimator->endSubAnim();
    mainPlayer->mAnimator->startAnimDead();
}

void killPlayer(PlayerActorHakoniwa* mainPlayer) {
    GameDataFunction::killPlayer(GameDataHolderWriter(mainPlayer));
    mainPlayer->startDemoPuppetable();
    al::setVelocityZero(mainPlayer);
    mainPlayer->mAnimator->endSubAnim();
    mainPlayer->mAnimator->startAnimDead();
}

void warpPlayer(const char* stageName, GameDataHolderAccessor holder) {
    ChangeStageInfo info(holder.mData, "", stageName, false, -1, ChangeStageInfo::SubScenarioType::NO_SUB_SCENARIO);
    GameDataFunction::tryChangeNextStage(holder.mData, &info);
}
}  // namespace PlayerHelper

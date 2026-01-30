#pragma once

#include "game/Player/PlayerActorBase.h"
#include "game/Util/IUseDimension.h"

#include "HackCap.h"
#include "PlayerAnimator.h"
#include "PlayerColliderHakoniwa.h"
#include "PlayerConst.h"
#include "PlayerHackKeeper.h"
#include "PlayerInfo.h"
#include "PlayerInitInfo.h"
#include "PlayerInput.h"
#include "PlayerModelKeeper.h"
#include "PlayerPuppet.h"

class YukimaruRacePlayer : public PlayerActorBase, public IUseDimension {
public:
    void initPlayer(const al::ActorInitInfo&, const PlayerInitInfo&);
    undefined size[0x1A8];
};

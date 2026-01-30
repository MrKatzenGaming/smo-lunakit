#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace rs {
void initItemByPlacementInfo(al::LiveActor*, const al::ActorInitInfo&, bool);
float setShadowDropLength(al::LiveActor*, const al::ActorInitInfo&, const char*);
bool isSeparatePlay(const al::IUseSceneObjHolder*);
void changeSeparatePlayMode(al::Scene*, bool);
}  // namespace rs

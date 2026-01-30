#pragma once

#include "al/actor/ActorCameraTarget.h"

class PlayerCameraTarget : public al::ActorCameraTarget {
public:
    PlayerCameraTarget(const al::LiveActor* player);

    float unk1;
    float unk2;
};

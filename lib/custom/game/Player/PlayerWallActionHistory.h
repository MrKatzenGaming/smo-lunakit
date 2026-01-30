#pragma once

#include "sead/math/seadVector.h"

#include "game/Player/IUsePlayerCollision.h"

class PlayerWallActionHistory {
public:
    void recordWallJump(const IUsePlayerCollision*, const sead::Vector3<float>&);
    void recordWallJump(const sead::Vector3<float>&, const sead::Vector3<float>&);
    void recordWallLeave(const sead::Vector3<float>&, const sead::Vector3<float>&);
    void reset();
    void update(const IUsePlayerCollision*);

    const char unk1[4];
    sead::Vector3f mJumpPosition;
    sead::Vector3f mJumpNormal;
};

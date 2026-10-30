#pragma once

#include "Library/LiveActor/LiveActor.h"

#include "MapObj/RouteGuidePoint.h"

namespace al {
class PlacementId;
}  // namespace al

struct SaveObjInfo;
class MapObjStatePlayerHold;
class GrowSeedStateWait;
class GrowSeedStateRevival;
class PlayerHoldObjTutorialController;

class GrowPlantSeed : public al::LiveActor, public RouteGuidePoint {
public:
    // TODO: Methods

public:
    al::PlacementId* mPlacementId;
    SaveObjInfo* mSaveObjInfo;
    u8 _128[0x18];

    sead::Vector3f _140;

    MapObjStatePlayerHold* mHold;
    GrowSeedStateWait* mWait;
    GrowSeedStateWait* mWaitWater;
    GrowSeedStateRevival* mRevival;

    sead::Quatf mQuat;
    s32 _180;
    u32 mTrampleCooldown;

    bool mIsSave;
    bool _189;

    sead::Vector3f mEffectFollowPos;
    s32 _198;
    bool _19c;
    bool _19d;
    bool _19e;
    s32 _1a0;
    bool _1a4;

    PlayerHoldObjTutorialController* mTutorialController;
    bool _1b0;
};

static_assert(sizeof(GrowPlantSeed) == 0x1b8);

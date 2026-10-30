#pragma once

#include <math/seadVector.h>

namespace al {
class CameraTicket;
class LiveActor;
class EventFlowEventData;
class IUseCamera;
class EventFlowRequestInfo;
class EventFlowScareCtrlBase;
class BalloonOrderGroup;

class EventFlowActorTerritory {
public:
    EventFlowActorTerritory(al::LiveActor* actor, const char* suffix);

public:
    f32 mRadius;
    f32 mEmotionRadius;
    f32 mEmotionLowPriorityRadius;
    f32 mTalkableRadius;

    sead::Vector3f mTerritoryLocalOffset;
    sead::Vector3f mBalloonLocalOffset;
    f32 mBalloonCollisionCheckOffsetRadius;
};

static_assert(sizeof(EventFlowActorTerritory) == 0x2c);

}  // namespace al

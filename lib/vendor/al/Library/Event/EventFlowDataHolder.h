#pragma once

#include "Library/Camera/CameraUtil.h"
#include "Library/Event/IUseEventFlowData.h"

#include <basis/seadTypes.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>

namespace al {
class CameraTicket;
class LiveActor;
class EventFlowEventData;
class IUseCamera;
class EventFlowActorTerritory;
class EventFlowNodeHolder;
class EventFlowMovement;
class EventFlowRequestInfo;
class EventFlowScareCtrlBase;
class EventFlowWatchParam;
class BalloonOrderGroup;
class MessageTagDataHolder;

class EventFlowDataHolder {
public:
    struct CameraInfo {
        const char* name;
        CameraTicket* ticket;
        void* _10;
    };
    static_assert(sizeof(CameraInfo) == 0x18);

    EventFlowDataHolder();

    bool isExistCameraInfo(const char*) const;
    const char* getCameraName(s32) const;
    void initCamera(const char*, CameraTicket*);
    void initCameraSubTargetAfterChart(const LiveActor*);
    void stopMovementByNode();
    void restartMovementByNode();
    void initCharacterName(const char16*);
    void initItemType(s32);
    void addItemType(const char*);
    void setActorFront(const sead::Vector3f&);
    const char* tryGetCharacterName() const;
    void convertActionName(sead::BufferedSafeString*, const char*) const;
    s32 sendEvent(const EventFlowEventData*);
    const char* judgeQuery(const char*) const;
    void setTalkSubActorName(const char*);
    void tryCreateCameraInfoBySystem(const char*);
    void startEventCamera(IUseCamera*, const char*, s32);
    void startEventAnimCamera(IUseCamera*, const char*, const char*, s32);
    void endEventCamera(IUseCamera*, const char*, s32, bool);
    bool tryEndEventCameraIfPlaying(IUseCamera*, const char*, s32, bool);
    void endAllEventCamera(IUseCamera*);
    bool isEndInterpoleCamera(const IUseCamera*, const char*) const;
    bool isPlayingEventAnimCamera(const char*) const;

public:
    EventFlowRequestInfo* mRequestInfo;
    EventFlowNodeHolder* mNodeHolder;
    void* _10;
    void* _18;
    EventFlowWatchParam* mPlayerWatch;

    sead::Vector3f mActorFront;
    bool _34;
    bool _35;

    s32 mCameraNum;
    CameraInfo** mCameraInfos;

    ActorCameraSubTarget* mEventCamera;
    EventFlowActorTerritory* mActorTerritory;
    void* _58;
    EventFlowMovement* mMovement;
    EventFlowScareCtrlBase* mScareCtrl;
    void* _70;
    void* _78;
    MessageTagDataHolder* _80;
    BalloonOrderGroup* mBalloonOrderGroup;
    sead::WFixedSafeString<32> mCharacterName;
    sead::SafeString mTalkSubActorName;
    s32 mItemTypenNumMax;
    s32 mItemTypeNum;
    const char** mItemTypes;
};

static_assert(sizeof(EventFlowDataHolder) == 0x108);

}  // namespace al

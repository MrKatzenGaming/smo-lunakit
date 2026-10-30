#pragma once

#include "Library/Event/IEventFlowEventReceiver.h"
#include "Library/Event/IEventFlowQueryJudge.h"
#include "Library/LiveActor/LiveActor.h"

#include <container/seadPtrArray.h>
#include <prim/seadSafeString.h>

namespace al {
class EventFlowExecutor;
class PlacementId;
}  // namespace al

class FukuwaraiWatcher;
class NpcStateReaction;
class NpcJointLookAtController;
class TalkNpcCap;
class TalkNpcParam;

class FukuwaraiNpc : public al::LiveActor, public al::IEventFlowEventReceiver, public al::IEventFlowQueryJudge {
public:
    FukuwaraiNpc(const char* name);

    void init(const al::ActorInitInfo& initInfo) override;
    bool getNextLv() const;

    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) override;
    void control() override;

    bool receiveEvent(const al::EventFlowEventData* event) override;
    const char* judgeQuery(const char* query) const override;

    void exeReaction();
    void exeWait();
    void exeMemorize();
    void exeEventMemorize();
    void exePlay();

    void startResultWipe();
    void exeEventStartResult();
    void exeEventStartResultEnd();

    void startResult(s32 score);
    bool isCompleteKuriboMario() const;
    void startEndResult();
    void reset();

    void exeEventResult();
    void exeEventEnd();

    bool isEventStartResultEnd() const;
    bool isEventResultJudgeEnd() const;
    bool isMemorize() const;
    bool isSetStartPosition() const;
    bool isPlay() const;
    bool isWaitStartResult() const;
    bool isEnd() const;

public:
    sead::FixedSafeString<128> _118;

    al::EventFlowExecutor* mEventFlow;
    FukuwaraiWatcher* mWatcher;
    void* _1c0;

    sead::Quatf mStartQuat;
    sead::Quatf mStartTrans;

    s32 mScore;
    s32 mSuccessScore;

    al::PlacementId* mShineActor;
    al::PlacementId* mShineActorMoonLockOpened;

    bool mIsMario;
    bool mIsCompleteKuriboMario;

    NpcStateReaction* mReaction;
    TalkNpcParam* mTalkNpcParam;
    NpcJointLookAtController* mNpcJointLookAtController;
    TalkNpcCap* mTalkNpcCap;
};

static_assert(sizeof(FukuwaraiNpc) == 0x228);

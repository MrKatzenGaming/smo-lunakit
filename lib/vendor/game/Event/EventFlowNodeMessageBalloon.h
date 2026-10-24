#pragma once

#include "Library/Event/EventFlowNode.h"

class NpcEventBalloonRequestInfo;

class EventFlowNodeMessageBalloon : public al::EventFlowNode {
public:
    EventFlowNodeMessageBalloon(const char* name) : EventFlowNode(name) {}

    void init(const al::EventFlowNodeInitInfo& info) override;
    void initAfterPlacement() override;

    void start() override;
    void control() override;
    s32 getNextId() const override;

    void exeWait();
    const char16* getMessage();

    void exeScare();
    void exeTrigger();

    bool isEnableEndFork();

public:
    NpcEventBalloonRequestInfo* mBalloonRequestInfo;

    const char16* mMessage;
    const char16* mTransparentCapMessage;

    bool mIsTalkBalloon;
    bool mIsEnableTalkHacking;
    bool mIsDisableTalkInWater;
    bool mIsMultiDevide;  // TYPO: Should be 'mIsMultiDivide'
    bool mIsIconBalloon;
    bool mIsShowAlways;
    bool mIsShowOnlyFaceToCameraFront;
    bool mIsHideIfExistTutorial;
    bool mIsLowPriority;

    const char* mEmotionType;
    u32 mIsMiniGame;
};

static_assert(sizeof(EventFlowNodeMessageBalloon) == 0xa0);

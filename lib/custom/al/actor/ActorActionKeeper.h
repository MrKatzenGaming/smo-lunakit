#pragma once

#include "al/Library/LiveActor/LiveActor.h"

namespace al {

class ActorResource;
class ActionAnimCtrl;
class NerveActionCtrl;
class ActionFlagCtrl;
class ActionSeCtrl;
class ActionBgmCtrl;
class ActionEffectCtrl;

class ActionPadAndCameraCtrl {
public:
    ActionPadAndCameraCtrl(const LiveActor*, const al::ActorResource*, const sead::Vector3f*, const char*);
    unsigned char padding[0x18];
    int mRumbleCount;  // 0x18
};
class ActionScreenEffectCtrl;

class ActorActionKeeper {
public:
    inline ActorActionKeeper(LiveActor*, const char*, ActionAnimCtrl*, NerveActionCtrl*, ActionFlagCtrl*, ActionEffectCtrl*, ActionSeCtrl*,
                             ActionBgmCtrl*, ActionPadAndCameraCtrl*, ActionScreenEffectCtrl*);
    void tryCreate(al::LiveActor*, const al::ActorResource*, const char*, const char*);
    void startAction(const char*);
    void tryStartActionNoAnim(const char*);
    void updatePrev(void);
    void updatePost(void);
    void init(void);

    LiveActor* mParentActor;                    // 0x0
    const char* mActorName;                     // 0x8
    bool unkBool;                               // 0x10
    ActionAnimCtrl* mAnimCtrl;                  // 0x18
    NerveActionCtrl* mNrvActionCtrl;            // 0x20
    ActionFlagCtrl* mFlagCtrl;                  // 0x28
    ActionEffectCtrl* mEffectCtrl;              // 0x30
    ActionSeCtrl* mSeCtrl;                      // 0x38
    ActionScreenEffectCtrl* mScreenEffectCtrl;  // 0x40
    ActionPadAndCameraCtrl* mPadAndCamCtrl;     // 0x48
    ActionBgmCtrl* mBgmCtrl;                    // 0x50
};
}  // namespace al

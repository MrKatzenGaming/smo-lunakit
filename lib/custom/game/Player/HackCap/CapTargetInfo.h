#pragma once

#include "Library/LiveActor/LiveActor.h"

class CapTargetInfo {
public:
    CapTargetInfo(void);
    void init(const al::LiveActor*, const char*);
    void setFollowLockOnMtx(const char*, const sead::Vector3f&, const sead::Vector3f&);
    void setLockOnStartAnimName(const char*);
    void setLockOnAnimName(const char*);
    void setHackName(const char*);
    void makeLockOnMtx(sead::Matrix34f*);
    void calcLockOnFollowTargetScale(sead::Vector3f*);
};

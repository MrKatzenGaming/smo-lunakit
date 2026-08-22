#pragma once

#include "sead/controller/seadControllerAddon.h"
#include "sead/math/seadVector.h"

namespace al {
class PadGyroAddon : public sead::ControllerAddon {
    SEAD_RTTI_OVERRIDE(PadGyroAddon, sead::ControllerAddon);
    PadGyroAddon(sead::Controller*, int);
    ~PadGyroAddon() override;

public:
    bool calc() override;
    bool tryUpdateGyroStatus();
    void getPose(sead::Vector3f* outSide, sead::Vector3f* outUp, sead::Vector3f* outFront) const;

public:
    bool mGyroStatus = false;
    sead::Vector3f mSide = {1.0f, 0.0f, 0.0f};   // ex
    sead::Vector3f mUp = {0.0f, 1.0f, 0.0f};     // ey
    sead::Vector3f mFront = {0.0f, 0.0f, 1.0f};  // ez
    sead::Vector3f mAngularVelocity = {0.0f, 0.0f, 0.0f};
    sead::Vector3f mAngle = {0.0f, 0.0f, 0.0f};
    s64 mPrevSamplingNumber = 0;
    s32 mSampleCount = 0;
    s32 mIndex;
};

}  // namespace al

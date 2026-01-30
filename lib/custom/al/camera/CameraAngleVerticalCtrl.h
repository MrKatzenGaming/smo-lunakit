#pragma once

#include "sead/math/seadVector.h"

#include "al/byaml/ByamlIter.h"
#include "al/nerve/NerveExecutor.h"

struct CameraAngleUpdateInfo;

namespace al {
class CameraAngleVerticalCtrl : public al::NerveExecutor {
public:
    CameraAngleVerticalCtrl(void);
    virtual ~CameraAngleVerticalCtrl();

    void getInitDefaultAngleDegree(void);
    void loadParam(const al::ByamlIter&);
    void start(const sead::Vector3f&);
    void startUserCtrl(void);
    void update(const CameraAngleUpdateInfo&);
    void getDefaultAngleDegree(void);
    void setAngleDegree(float);
    void startTargetInterpole(float);
    void startTargetInterpoleByStep(float, int);
    void startResetInterpole(void);
    void startResetInterpoleByStep(int);
    void chaseToTargetDegree(float);
    void chaseToTargetDegreeBySpeed(float, float);
    bool isFixInRange(void) const;
    void setRailAngleDegreeRangeAndInterp(float, float, int);
    void resetRailAngleDegreeRange(void);
    void startWaterCtrl(int);
    void invalidateAutoResetLowAngleV(void);
    void startSnap(float);
    void endSnap(void);

    void exeUserCtrl(void);
    void exeWaterCtrl(void);
    void exeHackFlyerCtrl(void);
    void exeInterp(void);
    void exeSnapStart(void);
    void exeSnap(void);
    void exeSnapEnd(void);
};
}  // namespace al

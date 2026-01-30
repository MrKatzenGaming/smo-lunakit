#pragma once

#include "sead/math/seadVector.h"

#include "al/camera/CameraPoser.h"
#include "al/camera/alCameraPoserFunction.h"

namespace al {
class CameraPoserFix : public CameraPoser {
public:
    CameraPoserFix(const char*);
    virtual void init(void) override;
    virtual void loadParam(const al::ByamlIter&) override;
    virtual void start(const al::CameraStartInfo&) override;
    virtual void update(void) override;

    void initCameraPosAndLookAtPos(const sead::Vector3f&, const sead::Vector3f&);
    const char* getFixAbsoluteCameraName(void);
    const char* getFixDoorwayCameraName(void);

    sead::Vector3f mLookAtPos;  // 0x140
    float mDistance;
    float mAngleV;
    float mAngleH;
    bool mIsCalcNearestAtFromPreAt;
    sead::Vector3f mPreLookAtPos;
};
}  // namespace al
